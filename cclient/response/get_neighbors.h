/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_NEIGHBORS_H
#define CCLIENT_RESPONSE_GET_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * URI of your peer.
   */
  char_buffer_t* address;
  /**
   * Number of all transactions sent (invalid, valid, already-seen).
   */
  int numberOfAllTransactions;
  /**
   * Invalid transactions your peer has sent you. These are transactions with
   * invalid signatures or overall schema.
   */
  int numberOfInvalidTransactions;
  /**
   * Number of newly transmitted transactions.
   */
  int numberOfNewTransactions;
} neighbor_t;

typedef UT_array get_neighbors_res_t;

void neighbor_t_copy(void* _dst, const void* _src);
void neighbor_t_dtor(void* _elt);

get_neighbors_res_t* get_neighbors_res_new();
void get_neighbors_res_free(get_neighbors_res_t* nbors);
neighbor_t* get_neighbors_res_create_neighbor(char_buffer_t* addr, int allTrans,
                                              int invalidTrans, int newTrans);
size_t get_neighbors_res_num(get_neighbors_res_t* nbors);
neighbor_t* get_neighbors_res_neighbor_at(get_neighbors_res_t* nbors,
                                          int index);
void get_neighbors_res_dump(get_neighbors_res_t* nbors);
retcode_t get_neighbors_res_add_neighbor(get_neighbors_res_t* nbors,
                                         char_buffer_t* addr, int allTrans,
                                         int invalidTrans, int newTrans);
#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NEIGHBORS_H
