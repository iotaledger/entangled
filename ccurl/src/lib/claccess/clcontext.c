#include "clcontext.h"
#include <stdio.h>
#include <string.h>

//#define _CL_ALL_
#ifndef DEBUG
#define DEBUG
#endif

static void CL_CALLBACK pfn_notify(const char* errinfo,
                                   const void* private_info, size_t cb,
                                   void* user_data) {
  fprintf(stderr, "W: ocl_pfn_notify error: %s\n", errinfo);
}
static void CL_CALLBACK bfn_notify(cl_program prog, void* user_data) {
  // fprintf(stderr, "W: build error: \n");
}

/*
int check_clerror(cl_int err, char *comment, ...) {
        if(err == CL_SUCCESS) {
                return 0;
        }
        printf("E: OpenCL implementation returned an error: %d\n", err);
        va_list args;
        vprintf(comment, args);
        printf("\n\n");
        return 1;
}
*/

void free_platforms(cl_platform_id* platforms, cl_uint num_platforms) {
  for (cl_uint i = 0; i < num_platforms; i++) {
    clUnloadPlatformCompiler(platforms[i]);
  }
  free(platforms);
}

static int get_devices(CLContext* ctx, unsigned char** src, size_t* size) {
  /* List devices for each platforms.*/
  size_t i;
  cl_uint num_platforms = 0;
  cl_int errno;
  ctx->num_devices = 0;
  cl_platform_id* platforms;
// cl_device_id devices[CLCONTEXT_MAX_DEVICES];

#ifdef DEBUG
  fprintf(stderr, "Getting platforms... \n");
#endif
  errno = clGetPlatformIDs(0, NULL, &num_platforms);
  if (errno != CL_SUCCESS) {
#ifdef DEBUG
    fprintf(stderr,
            "Cannot get the number of OpenCL platforms available. E:%d\n",
            errno);
#endif
    return 1;
  }
  // cl_platform_id platforms[num_platforms];
  platforms = malloc(num_platforms * sizeof(cl_platform_id));
  clGetPlatformIDs(num_platforms, platforms, NULL);
  for (i = 0; i < num_platforms; i++) {
    cl_uint pf_num_devices;
#ifdef _CL_ALL_
    if (clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL,
#else
    if (clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU,
#endif
                       CLCONTEXT_MAX_DEVICES - ctx->num_devices,
                       &(ctx->device[ctx->num_devices]),
                       &pf_num_devices) != CL_SUCCESS) {
#ifdef DEBUG
      fprintf(stderr, "W: Failed to get Opencl Device IDs for platform %zu.\n",
              i);
#endif
      continue;
    }
    if (pf_num_devices > CLCONTEXT_MAX_DEVICES - ctx->num_devices) {
      /*
              fprintf(stderr, "W: The number of devices available on your system
         \
                              exceeds CLCONTEXT_MAX_DEVICES. Consider increasing
         \
                              CLCONTEXT_MAX_DEVICES.\n");
                              */
      pf_num_devices = CLCONTEXT_MAX_DEVICES - ctx->num_devices;
    }
    ctx->num_devices += pf_num_devices;
  }
  if (ctx->num_devices == 0) {
    free_platforms(platforms, num_platforms);
    return 1;
  }

  /* Create OpenCL context. */
  for (i = 0; i < ctx->num_devices; i++) {
    ctx->clctx[i] = (cl_context)clCreateContext(NULL, 1, &(ctx->device[i]),
                                                pfn_notify, NULL, &errno);
    if (errno != CL_SUCCESS) {
#ifdef DEBUG
      fprintf(stderr, "Failed to execute clCreateContext, %d\n", errno);
#endif
      free_platforms(platforms, num_platforms);
      return 1;
    }
  }
  /* Get Device info */
  for (i = 0; i < ctx->num_devices; i++) {
    if (CL_SUCCESS !=
        clGetDeviceInfo(ctx->device[i], CL_DEVICE_MAX_COMPUTE_UNITS,
                        sizeof(cl_uint), &(ctx->num_cores[i]), NULL)) {
#ifdef DEBUG
      fprintf(stderr, "Failed to execute clGetDeviceInfo for %zu", i);
#endif
    }

    if (CL_SUCCESS !=
        clGetDeviceInfo(ctx->device[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE,
                        sizeof(cl_ulong), &(ctx->max_memory[i]), NULL)) {
#ifdef DEBUG
      fprintf(stderr, "Failed to execute clGetDeviceInfo for %zu", i);
#endif
    }

    if (clGetDeviceInfo(ctx->device[i], CL_DEVICE_MAX_WORK_GROUP_SIZE,
                        sizeof(size_t), &(ctx->num_multiple[i]),
                        NULL) != CL_SUCCESS) {
      ctx->num_multiple[i] = 1;
    }
  }
  /* Create command queue */
  for (i = 0; i < ctx->num_devices; i++) {
    ctx->clcmdq[i] =
        clCreateCommandQueue(ctx->clctx[i], ctx->device[i], 0, &errno);
    if (errno != CL_SUCCESS) {
#ifdef DEBUG
      fprintf(stderr, "Failed to execute clCreateCommandQueue.\n");
#endif
      free_platforms(platforms, num_platforms);
      return 1;
    }
  }

  if (ctx->kernel.num_src == 0) {
    free_platforms(platforms, num_platforms);
    return 1;
  }
  for (i = 0; i < ctx->num_devices; i++) {
    ctx->programs[i] = clCreateProgramWithSource(
        ctx->clctx[i], ctx->kernel.num_src, (const char**)src, size, &errno);
    if (CL_SUCCESS != errno) {
#ifdef DEBUG
      fprintf(stderr, "Failed to execute clCreateProgramWithSource\n");
#endif
      free_platforms(platforms, num_platforms);
      return 1;
    }
  }

  for (i = 0; i < ctx->num_devices; i++) {
    // memset(&(ctx->programs[i]), 0, sizeof(cl_program));
    errno = clBuildProgram(ctx->programs[i], 1, &(ctx->device[i]), "-Werror",
                           bfn_notify, NULL);
    char* build_log = malloc(0xFFFF);
    size_t log_size;
    clGetProgramBuildInfo(ctx->programs[i], ctx->device[i],
                          CL_PROGRAM_BUILD_LOG, 0xFFFF, build_log, &log_size);
#ifdef DEBUG
    fputs(build_log, stderr);
#endif
    free(build_log);
    if (CL_SUCCESS != errno) {
#ifdef DEBUG
      fprintf(stderr, "Failed to execute clBuildProgram\n");
#endif
      free_platforms(platforms, num_platforms);
      return 1;
    }
  }
  return 0;
}

static int create_kernel(CLContext* ctx, char** names) {
  // Create kernel.
  cl_int errno;
  size_t i, j;
  for (i = 0; i < ctx->num_devices; i++) {
    for (j = 0; j < ctx->kernel.num_kernels; j++) {
      ctx->clkernel[i][j] = clCreateKernel(ctx->programs[i], names[j], &errno);
      if (errno != CL_SUCCESS) {
        return 1;
      }
    }
  }
  return 0;
}

int kernel_init_buffers(CLContext* ctx) {
  int i, j, k;
  cl_ulong memsize;
  cl_int errno;
  cl_ulong maxmemsize;
  for (i = 0; i < ctx->num_devices; i++) {
    maxmemsize = 0;
    for (j = 0; j < ctx->kernel.num_buffers; j++) {
      memsize = ctx->kernel.buffer[j].size;
      if (ctx->kernel.buffer[j].init_flag & 2) {
        memsize *= ctx->num_cores[i] * ctx->num_multiple[i];
        if (memsize > ctx->max_memory[i]) {
          k = ctx->max_memory[i] / ctx->kernel.buffer[j].size;
          ctx->num_cores[i] = k;
          memsize = k * ctx->kernel.buffer[j].size;
          // fprintf(stderr, "resizing number of cores to %d...\n", k); //
          // ctx->max_memory[i]);
        }
        // fprintf(stderr, "size found: %ld, size allowed, %ld...\n", memsize,
        // ctx->max_memory[i]);
      }
      maxmemsize += memsize;
      if (maxmemsize >= ctx->max_memory[i]) {
#ifdef DEBUG
        fprintf(stderr, "E: max memory passed! \n");
#endif
        return 1;
      }

      ctx->buffers[i][j] = clCreateBuffer(
          ctx->clctx[i], ctx->kernel.buffer[j].flags, memsize, NULL, &errno);
      if (CL_SUCCESS != errno) {
#ifdef DEBUG
        fprintf(stderr, "Failed to execute clCreateBuffer for %d:%d", i, j);
#endif
        return 1;
      }
      for (k = 0; k < ctx->kernel.num_kernels; k++) {
        if ((ctx->kernel.buffer[j].init_flag & 1) != 0) {
          if (clSetKernelArg(ctx->clkernel[i][k], j, sizeof(cl_mem), NULL) !=
              CL_SUCCESS) {
            // fprintf(stderr, "Failed to execute clSetKernelArg for local
            // %d:%d", (int)i, (int)j);
            return 1;
          }
        } else {
          if (clSetKernelArg(ctx->clkernel[i][k], j, sizeof(cl_mem),
                             (void*)&(ctx->buffers[i][j])) != CL_SUCCESS) {
            // fprintf(stderr, "Failed to execute clSetKernelArg for %d:%d",
            // (int)i,(int)j);
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

int init_kernel(CLContext* ctx, char** names) {
  return create_kernel(ctx, names);
}

int pd_init_cl(CLContext* ctx, unsigned char** src, size_t* size,
               char** names) {
  if (!ctx) {
    ctx = malloc(sizeof(CLContext));
  }
  if (get_devices(ctx, src, size) != 0) {
    return 1;
  }
  return init_kernel(ctx, names);
}

void destroy_cl(CLContext* ctx) {
  size_t i;
  for (i = 0; i < ctx->num_devices; i++) {
    clFlush(ctx->clcmdq[i]);
    clFinish(ctx->clcmdq[i]);
  }
}

void finalize_cl(CLContext* ctx) {
  size_t i, j;
  destroy_cl(ctx);
  for (i = 0; i < ctx->num_devices; i++) {
    if (&(ctx->kernel) != NULL) {
      for (j = 0; j < ctx->kernel.num_kernels; j++) {
        clReleaseKernel(ctx->clkernel[i][j]);
      }
      if (ctx->kernel.num_src > 0)
        clReleaseProgram(ctx->programs[i]);
      for (j = 0; j < ctx->kernel.num_buffers; j++) {
        clReleaseMemObject(ctx->buffers[i][j]);
      }
    }
    clReleaseCommandQueue(ctx->clcmdq[i]);
    clReleaseContext(ctx->clctx[i]);
    clReleaseDevice(ctx->device[i]);
  }
  stubOpenclReset();
}
