/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_MODEL_TRANSFER_H_
#define __COMMON_MODEL_TRANSFER_H_

#include "common/errors.h"
#include "common/model/transaction.h"

/**
 * @brief A transfer object
 *
 */
typedef struct transfer_s {
  int64_t value;
  uint64_t timestamp;
  size_t msg_len;
  tryte_t *message;
  flex_trit_t address[FLEX_TRIT_SIZE_243];
  flex_trit_t tag[FLEX_TRIT_SIZE_81];
} transfer_t;

typedef UT_array signature_fragments_t;

static inline signature_fragments_t *signature_fragments_new() {
  signature_fragments_t *fragments = NULL;
  utarray_new(fragments, &ut_str_icd);
  return fragments;
}

static inline void signature_fragments_add(signature_fragments_t *fragments, tryte_t const *msg_or_sign) {
  utarray_push_back(fragments, &msg_or_sign);
}

static inline void signature_fragments_free(signature_fragments_t *fragments) { utarray_free(fragments); }
static inline tryte_t **signature_fragments_at(signature_fragments_t const *const fragments, size_t index) {
  return (tryte_t **)utarray_eltptr(fragments, index);
}

#define SIGNATURE_FRAGMENTS_FOREACH(fragments, fg) \
  for (fg = (tryte_t **)utarray_front(fragments); fg != NULL; fg = (tryte_t **)utarray_next(fragments, fg))

typedef UT_array transfer_array_t;

retcode_t transfer_message_set_trytes(transfer_t *tf, tryte_t const *const trytes, size_t length);
retcode_t transfer_message_set_string(transfer_t *tf, char const *const str);
static inline void transfer_message_free(transfer_t *tf) { free(tf->message); }
static inline tryte_t *transfer_message_get(transfer_t const *const tf) { return tf->message; }

transfer_array_t *transfer_array_new();
static inline void transfer_array_free(transfer_array_t *transfers) { utarray_free(transfers); }
static inline size_t transfer_array_count(transfer_array_t const *const transfers) { return utarray_len(transfers); }
static inline void transfer_array_add(transfer_array_t *tfs, transfer_t *tf) { utarray_push_back(tfs, tf); }
static inline transfer_t *transfer_array_at(transfer_array_t const *const tfs, size_t index) {
  return (transfer_t *)utarray_eltptr(tfs, index);
}

#define TRANSFER_FOREACH(tfs, tf) \
  for (tf = (transfer_t *)utarray_front(tfs); tf != NULL; tf = (transfer_t *)utarray_next(tfs, tf))

#endif  // __COMMON_MODEL_TRANSFER_H_

#ifdef __cplusplus
}
#endif
