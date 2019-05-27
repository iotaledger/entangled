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
#ifndef CCLIENT_RESPONSE_ADD_NEIGHBORS_H
#define CCLIENT_RESPONSE_ADD_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of add neighbors response
 *
 */
typedef struct add_neighbors_res_s {
  int added_neighbors; /**< Numbers of neighbors on this connected node*/
} add_neighbors_res_t;

/**
 * @brief Allocates an add neighbors response object
 *
 * @return A pointer to the response object.
 */
add_neighbors_res_t* add_neighbors_res_new();

/**
 * @brief Frees an add neighbors response object
 *
 * @param[in] res a response object.
 */
void add_neighbors_res_free(add_neighbors_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ADD_NEIGHBORS_H

/** @} */