/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/response/get_neighbors.h"

void neighbor_info_t_copy(void* _dst, const void* _src) {
  neighbor_info_t *dst = (neighbor_info_t*)_dst, *src = (neighbor_info_t*)_src;
  dst->address = src->address;
  dst->all_trans_num = src->all_trans_num;
  dst->random_trans_req_num = src->random_trans_req_num;
  dst->new_trans_num = src->new_trans_num;
  dst->invalid_trans_num = src->invalid_trans_num;
  dst->stale_trans_num = src->stale_trans_num;
  dst->sent_trans_num = src->sent_trans_num;
  dst->connection_type = src->connection_type;
}

void neighbor_info_t_dtor(void* _elt) {
  neighbor_info_t* elt = (neighbor_info_t*)_elt;
  char_buffer_free(elt->address);
  char_buffer_free(elt->connection_type);
}

UT_icd ut_neighbor_icd = {sizeof(neighbor_info_t), NULL, neighbor_info_t_copy, neighbor_info_t_dtor};

get_neighbors_res_t* get_neighbors_res_new() {
  get_neighbors_res_t* nbors = NULL;
  utarray_new(nbors, &ut_neighbor_icd);
  return nbors;
}

void get_neighbors_res_free(get_neighbors_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  utarray_free(*res);
  *res = NULL;
}

retcode_t get_neighbors_res_add_neighbor(get_neighbors_res_t* res, char const* const addr, uint32_t all_trans_num,
                                         uint32_t random_trans_req_num, uint32_t new_trans_num,
                                         uint32_t invalid_trans_num, uint32_t stale_trans_num, uint32_t sent_trans_num,
                                         char const* const connection_type) {
  if (!res) {
    return RC_NULL_PARAM;
  }

  neighbor_info_t nb = {.all_trans_num = all_trans_num,
                        .random_trans_req_num = random_trans_req_num,
                        .new_trans_num = new_trans_num,
                        .invalid_trans_num = invalid_trans_num,
                        .stale_trans_num = stale_trans_num,
                        .sent_trans_num = sent_trans_num};
  nb.address = char_buffer_new();
  nb.connection_type = char_buffer_new();
  if (!nb.address || !nb.connection_type) {
    return RC_OOM;
  }
  char_buffer_set(nb.address, addr);
  char_buffer_set(nb.connection_type, connection_type);

  utarray_push_back(res, &nb);
  return RC_OK;
}

neighbor_info_t* get_neighbors_res_neighbor_at(get_neighbors_res_t* res, size_t index) {
  if (!res) {
    return NULL;
  }
  if (utarray_len(res) > index) {
    neighbor_info_t* p = NULL;
    p = (neighbor_info_t*)utarray_eltptr(res, index);
    if (p) {
      return p;
    }
  }
  return NULL;
}

void get_neighbors_res_dump(get_neighbors_res_t* res) {
  if (!res) {
    return;
  }

  printf("neighbors %d\n", utarray_len(res));
  neighbor_info_t* nb = NULL;
  for (nb = (neighbor_info_t*)utarray_front(res); nb != NULL; nb = (neighbor_info_t*)utarray_next(res, nb)) {
    printf("index at %p\n", nb);

    printf(
        "address: %s \n"
        "numberOfAllTransactions: %d \n"
        "numberOfRandomTransactionRequests: %d \n"
        "numberOfNewTransactions: %d \n"
        "numberOfInvalidTransactions: %d \n"
        "numberOfStaleTransactions: %d \n"
        "numberOfSentTransactions: %d \n"
        "connectiontype: %s \n",
        nb->address->data, nb->all_trans_num, nb->random_trans_req_num, nb->new_trans_num, nb->invalid_trans_num,
        nb->stale_trans_num, nb->sent_trans_num, nb->connection_type->data);
  }
}
