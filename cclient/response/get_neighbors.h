/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_NEIGHBORS_H
#define CCLIENT_RESPONSE_GET_NEIGHBORS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * URI of your peer.
   */
  char_buffer_t* address;
  /**
   * The total number of transactions which includes sent, stale, etc.
   */
  uint32_t all_trans_num;
  /**
   *
   */
  uint32_t random_trans_req_num;
  /**
   * Number of newly transmitted transactions.
   */
  uint32_t new_trans_num;
  /**
   * Invalid transactions your peer has sent you. These are transactions with
   * invalid signatures or overall schema.
   */
  uint32_t invalid_trans_num;
  /**
   * The number of stale transactions
   */
  uint32_t stale_trans_num;
  /**
   * The number of sent transactions.
   */
  uint32_t sent_trans_num;
  /**
   * Connection type of client. It could be TCP, etc.
   */
  char_buffer_t* connection_type;

} neighbor_info_t;

typedef UT_array get_neighbors_res_t;

void neighbor_info_t_copy(void* _dst, const void* _src);
void neighbor_info_t_dtor(void* _elt);

get_neighbors_res_t* get_neighbors_res_new();
void get_neighbors_res_free(get_neighbors_res_t* nbors);
static inline size_t get_neighbors_res_num(get_neighbors_res_t* nbors) { return utarray_len(nbors); }
neighbor_info_t* get_neighbors_res_neighbor_at(get_neighbors_res_t* nbors, int index);
void get_neighbors_res_dump(get_neighbors_res_t* nbors);
retcode_t get_neighbors_res_add_neighbor(get_neighbors_res_t* nbors, char const* const addr, uint32_t all_trans_num,
                                         uint32_t random_trans_req_num, uint32_t new_trans_num,
                                         uint32_t invalid_trans_num, uint32_t stale_trans_num, uint32_t sent_trans_num,
                                         char const* const connection_type);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NEIGHBORS_H
