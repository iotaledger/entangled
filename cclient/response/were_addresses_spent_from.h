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
#ifndef CCLIENT_RESPONSE_WERE_ADDRESSES_SPENT_FROM_H
#define CCLIENT_RESPONSE_WERE_ADDRESSES_SPENT_FROM_H

#include <stdbool.h>

#include "common/errors.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of were addresses spent from response.
 *
 */
typedef struct were_addresses_spent_from_res_s {
  /**
   * States of the given addresses in the parameter. A <b>true</b> value means that the given address was
   * spent. A <b>false</b> value means that the given address has not been spent.
   */
  UT_array* states;
} were_addresses_spent_from_res_t;

/**
 * @brief Allocates a were addresses spent from response object.
 *
 * @return A pointer to the response object.
 */
were_addresses_spent_from_res_t* were_addresses_spent_from_res_new();

/**
 * @brief Adds a state to the response object.
 *
 * @param[in] res The response object.
 * @param[in] state The state of the address.
 * @return #retcode_t
 */
retcode_t were_addresses_spent_from_res_states_add(were_addresses_spent_from_res_t* res, int state);

/**
 * @brief Frees a were addresses spent from response.
 *
 * @param[in] res the response object.
 */
void were_addresses_spent_from_res_free(were_addresses_spent_from_res_t** res);

/**
 * @brief Gets the address state by index.
 *
 * @param[in] res The response object.
 * @param[in] index An index of the list.
 * @return Boolean. <b>True</b> means the address was spent, otherwise <b>false</b>.
 */
bool were_addresses_spent_from_res_states_at(were_addresses_spent_from_res_t* res, size_t index);

/**
 * @brief Gets the size of the states list.
 *
 * @param[in] res The response object.
 * @return The size of the state list.
 */
size_t were_addresses_spent_from_res_states_count(were_addresses_spent_from_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_WERE_ADDRESSES_SPENT_FROM_H

/** @} */