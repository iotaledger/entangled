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

#include "common/errors.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the add neighbors request.
 *
 * The URI format:
 *  - tcp://IPADDRESS:PORT
 *  - udp://IPADDRESS:PORT
 *
 */
typedef struct add_neighbors_req_s {
  UT_array* uris; /**< List of neighbor URIs for adding */
} add_neighbors_req_t;

/**
 * @brief Allocates the add neighbors request object.
 *
 * @return A pointer to the request object.
 */
add_neighbors_req_t* add_neighbors_req_new();

/**
 * @brief Free the add neighbors request.
 *
 * @param[in] req The request object.
 */
void add_neighbors_req_free(add_neighbors_req_t** req);

/**
 * @brief Add an URI string to the request.
 *
 * @param[in] req The request object.
 * @param[in] uri An URI string. see \ref add_neighbors_req_t.uris
 * @return #retcode_t
 */
retcode_t add_neighbors_req_uris_add(add_neighbors_req_t* req, char const* const uri);

/**
 * @brief Get an URI string from the URI list.
 *
 * @param[in] req The request object
 * @param[in] idx The index of URI list.
 * @return A string of URI. see \ref add_neighbors_req_t.uris
 */
const char* add_neighbors_req_uris_at(add_neighbors_req_t* req, size_t idx);

/**
 * @brief The size of URI list
 *
 * @param[in] req request object
 * @return The number of URI elements in the list.
 */
size_t add_neighbors_req_uris_len(add_neighbors_req_t* req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ADD_NEIGHBORS_H

/** @} */