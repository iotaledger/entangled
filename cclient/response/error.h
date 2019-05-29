/*
 * Copyright (c) 2019 IOTA Stiftung
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
#ifndef CCLIENT_RESPONSE_ERROR_H
#define CCLIENT_RESPONSE_ERROR_H

#include "utils/char_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the error response.
 *
 */
typedef struct error_res_s {
  char_buffer_t* error; /*!< An error string */
} error_res_t;

/**
 * @brief Allocates an error response object.
 *
 * @param[in] error The error string for this error response.
 * @return A pointer to the response object.
 */
error_res_t* error_res_new(char const* const error);

/**
 * @brief Gets the error string.
 *
 * @param[in] res The response object.
 * @return A pointer to the error string.
 */
char* error_res_get_message(error_res_t const* const res);

/**
 * @brief Frees an error response object.
 *
 * @param[in] res the response object.
 */
void error_res_free(error_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ERROR_H

/** @} */