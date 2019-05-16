/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup common_model
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef __COMMON_MODEL_INPUTS_H__
#define __COMMON_MODEL_INPUTS_H__

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief input object
 *
 */
typedef struct {
  int64_t balance;
  uint64_t key_index;
  uint8_t security;
  flex_trit_t address[FLEX_TRIT_SIZE_243];
} input_t;

typedef UT_array input_array_t;

/**
 * @brief a list of input object
 *
 */
typedef struct {
  int64_t total_balance;
  input_array_t* input_array;
} inputs_t;

static UT_icd const input_array_icd = {sizeof(input_t), NULL, NULL, NULL};

/**
 * @brief append an input object to the input list
 *
 * @param inputs the input list
 * @param input an input object for appending
 * @return retcode_t
 */
static inline retcode_t inputs_append(inputs_t* inputs, input_t* input) {
  if (!inputs->input_array) {
    utarray_new(inputs->input_array, &input_array_icd);
  }
  if (!inputs->input_array) {
    return RC_OOM;
  }
  inputs->total_balance += input->balance;
  utarray_push_back(inputs->input_array, input);
  return RC_OK;
}

/**
 * @brief clear all elements
 *
 * @param inputs
 */
static inline void inputs_clear(inputs_t* inputs) {
  if (inputs->input_array) {
    utarray_free(inputs->input_array);
  }
}

/**
 * @brief total balance in this input list
 *
 * @param inputs
 * @return int64_t
 */
static inline int64_t inputs_balance(inputs_t* inputs) { return inputs->total_balance; }

/**
 * @brief size of the input list
 *
 * @param inputs
 * @return size_t
 */
static inline size_t inputs_len(inputs_t const* const inputs) {
  if (!inputs->input_array) {
    return 0;
  }
  return utarray_len(inputs->input_array);
}

/**
 * @brief get an input object by index
 *
 * @param inputs
 * @param index
 * @return input_t*
 */
static inline input_t* inputs_at(inputs_t* inputs, size_t index) {
  if (!inputs->input_array) {
    return NULL;
  }
  return (input_t*)utarray_eltptr(inputs->input_array, index);
}

/**
 * @brief loop over an input list
 *
 */
#define INPUTS_FOREACH(inputs, elm) \
  for (elm = (input_t*)utarray_front(inputs); elm != NULL; elm = (input_t*)utarray_next(inputs, elm))

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_MODEL_INPUTS_H__

/** @} */