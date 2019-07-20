/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup response
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_RESPONSE_GET_NEIGHBORS_H
#define CCLIENT_RESPONSE_GET_NEIGHBORS_H

#include <stdint.h>

#include "common/errors.h"
#include "utarray.h"
#include "utils/char_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the neighbor object.
 *
 */
typedef struct {
  /**
   * address and port number of your peer.
   */
  char_buffer_t* address;
  /**
   * Number of all transactions(Invalid, valid and already-seen) this neighbor sent to you.
   */
  uint32_t all_trans_num;
  /**
   * Number of random tips this neighbor requested from your node.
   */
  uint32_t random_trans_req_num;
  /**
   * Number of new transactions received from this neighbor. New transactions are transactions that you have not yet
   * stored when this neighbor sends it to you.
   */
  uint32_t new_trans_num;
  /**
   * Number of invalid transactions this neighbor sent to you. These are transactions with invalid signatures or overall
   * schema.
   */
  uint32_t invalid_trans_num;
  /**
   * Stale transactions this neighbor has sent you. These are transactions with a timestamp older than your latest
   * snapshot.
   */
  uint32_t stale_trans_num;
  /**
   * Amount of transactions sent through this neighbor.
   * Number of all transactions you get from other neighbors and forward to this neighbor.
   */
  uint32_t sent_trans_num;
  /**
   * The method type this neighbor is using to connect (TCP / UDP).
   */
  char_buffer_t* connection_type;

} neighbor_info_t;

/**
 * @brief The data structure of get neighbor response.
 *
 */
typedef UT_array get_neighbors_res_t;

/**
 * @brief Allocates a get neighbors response object.
 *
 * @return A pointer to the response object.
 */
get_neighbors_res_t* get_neighbors_res_new();

/**
 * @brief Frees a get neighbors response.
 *
 * @param[in] res The response object.
 */
void get_neighbors_res_free(get_neighbors_res_t** res);

/**
 * @brief Gets the number of neighbors in the response.
 *
 * @param[in] res The response object.
 * @return The number of neighbors.
 */
static inline size_t get_neighbors_res_num(get_neighbors_res_t* res) { return utarray_len(res); }

/**
 * @brief Gets a neighbor object by index.
 *
 * @param[in] res The response object.
 * @param[in] index An index of the neighbor list.
 * @return A pointer to a neighbor object.
 */
neighbor_info_t* get_neighbors_res_neighbor_at(get_neighbors_res_t* res, size_t index);

/**
 * @brief Adds a neighbor to the response.
 *
 * @param[in] res The response object.
 * @param[in] addr The string of an address.
 * @param[in] all_trans_num The value of \ref neighbor_info_t.all_trans_num
 * @param[in] random_trans_req_num The value of \ref neighbor_info_t.random_trans_req_num
 * @param[in] new_trans_num The value of \ref neighbor_info_t.new_trans_num
 * @param[in] invalid_trans_num The value of \ref neighbor_info_t.invalid_trans_num
 * @param[in] stale_trans_num The value of \ref neighbor_info_t.stale_trans_num
 * @param[in] sent_trans_num The value of \ref neighbor_info_t.sent_trans_num
 * @param[in] connection_type A string of <b>TCP</b> or <b>UDP</b>.
 * @return #retcode_t
 */
retcode_t get_neighbors_res_add_neighbor(get_neighbors_res_t* res, char const* const addr, uint32_t all_trans_num,
                                         uint32_t random_trans_req_num, uint32_t new_trans_num,
                                         uint32_t invalid_trans_num, uint32_t stale_trans_num, uint32_t sent_trans_num,
                                         char const* const connection_type);

#ifndef DOXYGEN_SKIP_THIS
/**
 * @brief Dumps neighbor info through stdout.
 *
 * @param[in] res The response object.
 */
void get_neighbors_res_dump(get_neighbors_res_t* res);
#endif /* DOXYGEN_SKIP_THIS */

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NEIGHBORS_H

/** @} */
