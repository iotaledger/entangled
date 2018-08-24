/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_neighbors.h"

void neighbor_t_copy(void* _dst, const void* _src) {
  neighbor_t *dst = (neighbor_t*)_dst, *src = (neighbor_t*)_src;
  dst->address = src->address;
  dst->all_trans_num = src->all_trans_num;
  dst->invalid_trans_num = src->invalid_trans_num;
  dst->new_trans_num = src->new_trans_num;
}

void neighbor_t_dtor(void* _elt) {
  neighbor_t* elt = (neighbor_t*)_elt;
  if (elt->address) {
    char_buffer_free(elt->address);
  }
}

UT_icd ut_neighbor_icd = {sizeof(neighbor_t), NULL, neighbor_t_copy,
                          neighbor_t_dtor};

get_neighbors_res_t* get_neighbors_res_new() {
  get_neighbors_res_t* nbors = malloc(sizeof(get_neighbors_res_t));
  if (nbors == NULL) {
    return NULL;
  }
  utarray_new(nbors, &ut_neighbor_icd);
  return nbors;
}

void get_neighbors_res_free(get_neighbors_res_t* nbors) { utarray_free(nbors); }

neighbor_t* get_neighbors_res_create_neighbor(char_buffer_t* addr,
                                              int all_trans, int invalid_trans,
                                              int new_trans) {
  neighbor_t* nb = malloc(sizeof(neighbor_t));
  if (nb == NULL) {
    return NULL;
  }
  nb->address = addr;
  nb->all_trans_num = all_trans;
  nb->invalid_trans_num = invalid_trans;
  nb->new_trans_num = new_trans;
  return nb;
}

retcode_t get_neighbors_res_add_neighbor(get_neighbors_res_t* nbors,
                                         char_buffer_t* addr, int all_trans,
                                         int invalid_trans, int new_trans) {
  retcode_t ret = RC_OK;
  neighbor_t* nb = malloc(sizeof(neighbor_t));
  if (nb == NULL) {
    return RC_CCLIENT_OOM;
  }
  nb->address = addr;
  nb->all_trans_num = all_trans;
  nb->invalid_trans_num = invalid_trans;
  nb->new_trans_num = new_trans;
  utarray_push_back(nbors, nb);
  return ret;
}

neighbor_t* get_neighbors_res_neighbor_at(get_neighbors_res_t* nbors,
                                          int index) {
  if (utarray_len(nbors) > index) {
    neighbor_t* p = NULL;
    p = (neighbor_t*)utarray_eltptr(nbors, index);
    if (p) {
      return p;
    }
  }
  return NULL;
}

void get_neighbors_res_dump(get_neighbors_res_t* nbors) {
  printf("neighbors %d\n", utarray_len(nbors));
  neighbor_t* nb = NULL;
  for (nb = (neighbor_t*)utarray_front(nbors); nb != NULL;
       nb = (neighbor_t*)utarray_next(nbors, nb)) {
    printf("index at %p\n", nb);
    printf(
        "address: %s \nnumberOfAllTransactions: %d\n"
        "numberOfInvalidTransactions: %d\nnumberOfNewTransactions: %d\n",
        nb->address->data, nb->all_trans_num, nb->invalid_trans_num,
        nb->new_trans_num);
  }
}
