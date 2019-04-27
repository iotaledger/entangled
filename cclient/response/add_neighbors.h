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

#include "utils/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// response is a number of neighbors.
/**
 * @brief Structure of add_neighbors response
 *
 */
typedef struct add_neighbors_res_s {
  int added_neighbors; /**< numbers of neighbors */
} add_neighbors_res_t;

/**
 * @brief New a add_neighbors response object
 *
 * @return add_neighbors_res_t*
 */
add_neighbors_res_t* add_neighbors_res_new();

/**
 * @brief Free a add_neighbors response object
 *
 * @param res response object
 */
void add_neighbors_res_free(add_neighbors_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ADD_NEIGHBORS_H

/** @} */