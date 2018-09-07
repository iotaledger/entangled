/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "attach_to_tangle.h"

attach_to_tangle_req_t* attach_to_tangle_req_new() {
  attach_to_tangle_req_t* req =
      (attach_to_tangle_req_t*)malloc(sizeof(attach_to_tangle_req_t));
  if (req) {
    req->trunk = char_buffer_new();
    req->branch = char_buffer_new();
    req->mwm = ATTACH_TO_TANGLE_MAIN_MWM;
    utarray_new(req->trytes, &ut_str_icd);
  }
  return req;
}

void attach_to_tangle_req_free(attach_to_tangle_req_t** req) {
  if (*req) {
    char_buffer_free((*req)->trunk);
    char_buffer_free((*req)->branch);
    utarray_free((*req)->trytes);
    free(*req);
    *req = NULL;
  }
}

retcode_t attach_to_tangle_req_set_trunk(attach_to_tangle_req_t* req,
                                         const char* trunk) {
  retcode_t ret = RC_OK;
  size_t str_len = strlen(trunk);
  ret = char_buffer_allocate(req->trunk, str_len);
  strncpy(req->trunk->data, trunk, str_len);
  return ret;
}

retcode_t attach_to_tangle_req_set_branch(attach_to_tangle_req_t* req,
                                          const char* branch) {
  retcode_t ret = RC_OK;
  size_t str_len = strlen(branch);
  ret = char_buffer_allocate(req->branch, str_len);
  strncpy(req->branch->data, branch, str_len);
  return ret;
}

void attach_to_tangle_req_set_mwm(attach_to_tangle_req_t* req,
                                  const int32_t mwm) {
  req->mwm = mwm;
}

void attach_to_tangle_req_add_trytes(attach_to_tangle_req_t* req,
                                     const char* trytes) {
  utarray_push_back(req->trytes, &trytes);
}
