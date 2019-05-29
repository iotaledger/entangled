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
#ifndef CCLIENT_RESPONSE_GET_INCLUSION_STATES_H
#define CCLIENT_RESPONSE_GET_INCLUSION_STATES_H

#include <stdbool.h>

#include "common/errors.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get inclusion states response.
 *
 */
typedef struct {
  /**
   * List of boolean values in the same order as the <b>transactions</b> parameters. A <b>true</b> value means the
   * transaction was confirmed.
   */
  UT_array* states;
} get_inclusion_states_res_t;

/**
 * @brief Allocates a get inclusion states response object.
 *
 * @return A pointer to the response object.
 */
get_inclusion_states_res_t* get_inclusion_states_res_new();

/**
 * @brief Adds a state to the response object.
 *
 * @param[in] res The response object.
 * @param[in] state The state of the transaction.
 * @return #retcode_t
 */
retcode_t get_inclusion_states_res_states_add(get_inclusion_states_res_t* res, int state);

/**
 * @brief Frees a get inclusion states response object.
 *
 * @param[in] res The response object.
 */
void get_inclusion_states_res_free(get_inclusion_states_res_t** res);

/**
 * @brief Gets the transaction state by index.
 *
 * @param[in] res The response object.
 * @param[in] index An index of the list.
 * @return Boolean. <b>True</b> means the transaction was confirmed, otherwise <b>false</b>.
 */
bool get_inclusion_states_res_states_at(get_inclusion_states_res_t* res, size_t index);

/**
 * @brief Gets the size of the states list.
 *
 * @param[in] res The response object.
 * @return The size of the state list.
 */
size_t get_inclusion_states_res_states_count(get_inclusion_states_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_INCLUSION_STATES_H

/** @} */
