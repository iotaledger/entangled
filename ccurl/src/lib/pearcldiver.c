#include "pearcldiver.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ccurl/pearl.cl.h"
#include "claccess/clcontext.h"
#include "constants.h"
#include "hash.h"
#include "pearl_diver.h"
#if defined(_WIN32) && !defined(__MINGW32__)
#else
#include <sched.h>
#endif

#ifndef PD_NUM_SRC
#define PD_NUM_SRC 1
#endif /*PD_NUM_SRC*/

typedef struct {
  States states;
  char* trits;
  curl_t* curl;
  size_t min_weight_magnitude;
  size_t index;
  size_t offset;
  PearCLDiver* pdcl;
} PDCLThread;

int init_pearcl(PearCLDiver* pdcl) {
  CLContext* cl;
  unsigned char* src[PD_NUM_SRC] = {ccurl_src_lib_cl_pearl_cl};
  size_t size[PD_NUM_SRC] = {ccurl_src_lib_cl_pearl_cl_len};
  char** names = (char*[]){"init", "search", "finalize"};
  if (!pdcl) {
    pdcl = malloc(sizeof(PearCLDiver));
  }
  cl = &(pdcl->cl);
  cl->kernel.num_src = PD_NUM_SRC;
  cl->kernel.num_kernels = 3;
  if (pd_init_cl(cl, src, size, names) != 0) {
    return 1;
  }
  cl->kernel.num_buffers = 9;
  cl->kernel.buffer[0] = (BufferInfo){sizeof(char) * HASH_LENGTH,
                                      CL_MEM_WRITE_ONLY};  // trit_hash //
  cl->kernel.buffer[1] = (BufferInfo){sizeof(bc_trit_t) * STATE_LENGTH,
                                      CL_MEM_READ_WRITE, 2};  // states array //
  cl->kernel.buffer[2] = (BufferInfo){sizeof(bc_trit_t) * STATE_LENGTH,
                                      CL_MEM_READ_WRITE, 2};  // states array //
  cl->kernel.buffer[3] = (BufferInfo){sizeof(bc_trit_t) * STATE_LENGTH,
                                      CL_MEM_READ_WRITE, 2};  // states array //
  cl->kernel.buffer[4] = (BufferInfo){sizeof(bc_trit_t) * STATE_LENGTH,
                                      CL_MEM_READ_WRITE, 2};  // states array //
  cl->kernel.buffer[5] =
      (BufferInfo){sizeof(size_t), CL_MEM_READ_ONLY};  // minweightmagnitude //
  cl->kernel.buffer[6] =
      (BufferInfo){sizeof(char), CL_MEM_READ_WRITE};  // found //
  cl->kernel.buffer[7] =
      (BufferInfo){sizeof(bc_trit_t), CL_MEM_READ_WRITE, 2};  // nonce_probe //
  cl->kernel.buffer[8] =
      (BufferInfo){sizeof(size_t), CL_MEM_READ_ONLY};  // loop_length //

  return kernel_init_buffers(cl);
}

void pearcl_write_buffers(PDCLThread* thread) {
  PearCLDiver* pdcl = thread->pdcl;
  CLContext* cl = &(pdcl->cl);
  cl_command_queue* cmdq = &(cl->clcmdq[thread->index]);
  cl_mem* mem = cl->buffers[thread->index];
  BufferInfo* bufinfo = cl->kernel.buffer;

  clEnqueueWriteBuffer(*cmdq, mem[1], CL_TRUE, 0, bufinfo[1].size,
                       &(thread->states.mid_low), 0, NULL, NULL);
  clEnqueueWriteBuffer(*cmdq, mem[2], CL_TRUE, 0, bufinfo[2].size,
                       &(thread->states.mid_high), 0, NULL, NULL);
  clEnqueueWriteBuffer(*cmdq, mem[5], CL_TRUE, 0, bufinfo[5].size,
                       &(thread->min_weight_magnitude), 0, NULL, NULL);
  clEnqueueWriteBuffer(*cmdq, mem[8], CL_TRUE, 0, bufinfo[8].size,
                       &(pdcl->loop_count), 0, NULL, NULL);
  /*
     fprintf(stderr, "E: failed to write mid state low");
     fprintf(stderr, "E: failed to write mid state low");
     fprintf(stderr, "E: failed to write min_weight_magnitude");
     fprintf(stderr, "E: failed to write min_weight_magnitude");
     */
}
#if defined(_WIN32) && !defined(__MINGW32__)
DWORD WINAPI pearcl_find(void* data) {
#else
void* pearcl_find(void* data) {
#endif
  size_t local_work_size, global_work_size, global_offset, num_groups;
  char found = 0;
  cl_event ev, ev1;
  PDCLThread* thread;
  PearCLDiver* pdcl;
  int i;
  thread = (PDCLThread*)data;
  global_offset = thread->offset;
  pdcl = thread->pdcl;
  num_groups = (pdcl->cl.num_cores[thread->index]);
  local_work_size = STATE_LENGTH;
  while (local_work_size > pdcl->cl.num_multiple[thread->index]) {
    local_work_size /= 3;
  }
  global_work_size = local_work_size * num_groups;

  for (i = 0; i < thread->index; i++) {
    global_offset += pdcl->cl.num_cores[i];
  }
  pearcl_write_buffers(thread);

  if (CL_SUCCESS == clEnqueueNDRangeKernel(pdcl->cl.clcmdq[thread->index],
                                           pdcl->cl.clkernel[thread->index][0],
                                           1, &global_offset, &global_work_size,
                                           &local_work_size, 0, NULL, &ev)) {
    clWaitForEvents(1, &ev);
    clReleaseEvent(ev);

    while (found == 0 && pdcl->pd.status == PD_SEARCHING) {
      if (clEnqueueNDRangeKernel(pdcl->cl.clcmdq[thread->index],
                                 pdcl->cl.clkernel[thread->index][1], 1, NULL,
                                 &global_work_size, &local_work_size, 0, NULL,
                                 &ev1) != CL_SUCCESS) {
        clReleaseEvent(ev1);
        fprintf(stderr,
                "E: "
                "running "
                "search "
                "kernel "
                "failed. "
                "\n");
        break;
      }
      clWaitForEvents(1, &ev1);
      clReleaseEvent(ev1);
      if (CL_SUCCESS != clEnqueueReadBuffer(pdcl->cl.clcmdq[thread->index],
                                            pdcl->cl.buffers[thread->index][6],
                                            CL_TRUE, 0, sizeof(char), &found, 0,
                                            NULL, NULL)) {
        fprintf(stderr,
                "E: "
                "reading "
                "finished "
                "bool "
                "failed."
                "\n");
        break;
      }
    }
  } else {
    fprintf(stderr, "E: running init kernel failed.\n");
    clReleaseEvent(ev);
  }

  if (CL_SUCCESS != clEnqueueNDRangeKernel(pdcl->cl.clcmdq[thread->index],
                                           pdcl->cl.clkernel[thread->index][2],
                                           1, NULL, &global_work_size,
                                           &local_work_size, 0, NULL, &ev))
    fprintf(stderr, "E: running finalize kernel failed.\n");

  if (found > 0) {
    pthread_mutex_lock(&pdcl->pd.new_thread_search);
    if (pdcl->pd.status != PD_FOUND) {
      pdcl->pd.status = PD_FOUND;
      if (CL_SUCCESS !=
          clEnqueueReadBuffer(pdcl->cl.clcmdq[thread->index],
                              pdcl->cl.buffers[thread->index][0], CL_TRUE, 0,
                              HASH_LENGTH * sizeof(char), &(thread->curl[0]),
                              //&(thread->trits[TRANSACTION_LENGTH
                              //- HASH_LENGTH]),
                              1, &ev, NULL)) {
      }
    }
    pthread_mutex_unlock(&pdcl->pd.new_thread_search);
  }
  clReleaseEvent(ev);
  return 0;
}

void pearcl_search(PearCLDiver* pdcl, curl_t* const curl, size_t offset,
                   size_t min_weight_magnitude) {
  int k, thread_count;
  int numberOfThreads = pdcl->cl.num_devices;

  if (min_weight_magnitude > HASH_LENGTH) {
    pdcl->pd.status = PD_INVALID;
    return;
  }

  pdcl->pd.status = PD_SEARCHING;

  States states;
  pd_search_init(&states, curl, HASH_LENGTH - NONCE_LENGTH);

  if (numberOfThreads == 0) {
    pdcl->pd.status = PD_FAILED;
    return;
  }

  pthread_mutex_init(&pdcl->pd.new_thread_search, NULL);
  pthread_t* tid = malloc(numberOfThreads * sizeof(pthread_t));
  thread_count = numberOfThreads;

  PDCLThread* pdthreads =
      (PDCLThread*)malloc(numberOfThreads * sizeof(PDCLThread));
  while (numberOfThreads-- > 0) {
    pdthreads[numberOfThreads] =
        (PDCLThread){.states = states,
                     .curl = curl,
                     .min_weight_magnitude = min_weight_magnitude,
                     .index = numberOfThreads,
                     .offset = offset,
                     .pdcl = pdcl};
    pthread_create(&tid[numberOfThreads], NULL, &pearcl_find,
                   (void*)&(pdthreads[numberOfThreads]));
  }

  sched_yield();
  for (k = thread_count; k > 0; k--) {
    pthread_join(tid[k - 1], NULL);
  }

  pthread_mutex_destroy(&pdcl->pd.new_thread_search);
  free(tid);
  free(pdthreads);
  // return pdcl->pd.interrupted;
}
