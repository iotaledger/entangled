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
#ifndef CCLIENT_REQUEST_REMOVE_NEIGHBORS_H
#define CCLIENT_REQUEST_REMOVE_NEIGHBORS_H

#include "common/errors.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the remove neighbors request.
 *
 * The URI format:
 *  - tcp://IPADDRESS:PORT
 *  - udp://IPADDRESS:PORT
 *
 */
typedef struct remove_neighbors_req_s {
  UT_array* uris; /**< List of neighbor URIs for removing */
} remove_neighbors_req_t;

/**
 * @brief Allocates a remove neighbors request object.
 *
 * @return A pointer to the request object.
 */
remove_neighbors_req_t* remove_neighbors_req_new();

/**
 * @brief Add a string URI to the request.
 *
 * @param[in] req The request object.
 * @param[in] uri A string of URI. see \ref remove_neighbors_req_t.uris
 * @return #retcode_t
 */
retcode_t remove_neighbors_req_add(remove_neighbors_req_t* req, char const* uri);

/**
 * @brief Free a remove neighbors request object.
 *
 * @param[in] req The request object.
 */
void remove_neighbors_req_free(remove_neighbors_req_t** req);

/**
 * @brief Get a URI string by index from the request.
 *
 * @param[in] req The request object.
 * @param[in] idx The index of URI list.
 * @return A URI string. see \ref remove_neighbors_req_t.uris
 */
char const* remove_neighbors_req_uris_at(remove_neighbors_req_t* req, size_t idx);

/**
 * @brief Get the size of URI list.
 *
 * @param[in] req The request object.
 * @return The number of URI elements in the list.
 */
size_t remove_neighbors_req_uris_len(remove_neighbors_req_t* req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_REMOVE_NEIGHBORS_H

/** @} */