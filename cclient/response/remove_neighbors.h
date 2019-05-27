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
#ifndef CCLIENT_RESPONSE_REMOVE_NEIGHBORS_H
#define CCLIENT_RESPONSE_REMOVE_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the remove neighbors response.
 *
 */
typedef struct remove_neighbors_res_s {
  int removed_neighbors; /*!< Total number of removed neighbors */
} remove_neighbors_res_t;

/**
 * @brief Allocates a remove neighbors response.
 *
 * @return A pointer to the response object.
 */
remove_neighbors_res_t* remove_neighbors_res_new();

/**
 * @brief Frees a remove neighbors response.
 *
 * @param[in] res The response object.
 */
void remove_neighbors_res_free(remove_neighbors_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_REMOVE_NEIGHBORS_H

/** @} */
