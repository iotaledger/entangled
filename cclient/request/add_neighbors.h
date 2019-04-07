/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup request
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_REQUEST_ADD_NEIGHBORS_H
#define CCLIENT_REQUEST_ADD_NEIGHBORS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure of add_neighbors request
 *
 */
typedef struct add_neighbors_req_s {
  UT_array* uris; /**< An URI array */
} add_neighbors_req_t;

/**
 * @brief new add_neighbors_req_t objects
 *
 * @return add_neighbors_req_t*
 */
add_neighbors_req_t* add_neighbors_req_new();

/**
 * @brief free add_neighbors_req_t
 *
 * @param req add_neighbors_req_t object
 */
void add_neighbors_req_free(add_neighbors_req_t** req);

/**
 * @brief add an URI string to request
 *
 * @param req request object
 * @param uri An URI string
 * @return An error code
 */
retcode_t add_neighbors_req_uris_add(add_neighbors_req_t* req, const char* uri);

/**
 * @brief return an URI string in the request list
 *
 * @param req request object
 * @param idx index of URI
 * @return string
 */
const char* add_neighbors_req_uris_at(add_neighbors_req_t* req, size_t idx);

/**
 * @brief URI number in the request list
 *
 * @param req request object
 * @return size_t
 */
size_t add_neighbors_req_uris_len(add_neighbors_req_t* req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ADD_NEIGHBORS_H

/** @} */