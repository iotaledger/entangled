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
#ifndef CCLIENT_RESPONSE_CHECK_CONSISTENCY_H
#define CCLIENT_RESPONSE_CHECK_CONSISTENCY_H

#include <stdbool.h>

#include "common/errors.h"
#include "utils/char_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of check consistency response.
 *
 */
typedef struct check_consistency_res_s {
  /**
   * State of the given transactions in the tails parameter. A <b>true</b> value means that all given transactions are
   * consistent. A <b>false</b> value means that one or more of the given transactions aren't consistent.
   */
  bool state;
  /**
   * If the <b>state</b> field is <b>false</b>, this field contains information about why the transaction is
   * inconsistent.
   *
   */
  char_buffer_t* info;
} check_consistency_res_t;

/**
 * @brief Allocates a check consistency response object.
 *
 * @return A pointer to the response object.
 */
check_consistency_res_t* check_consistency_res_new();

/**
 * @brief Sets the info field of the response object.
 *
 * @param[in] res The response object.
 * @param[in] info The info string.
 * @return #retcode_t
 */
retcode_t check_consistency_res_info_set(check_consistency_res_t* res, char const* const info);

/**
 * @brief Frees a check consistency response.
 *
 * @param[in] res the response object.
 */
void check_consistency_res_free(check_consistency_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_CHECK_CONSISTENCY_H

/** @} */