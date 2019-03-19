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

#include <stdlib.h>
#include <string.h>
#include "common/crypto/kerl/kerl.h"
#include "common/helpers/sign.h"
#include "common/model/transaction.h"
#include "common/trinary/trit_long.h"
#include "utils/logger_helper.h"

typedef enum { DATA = 0, VALUE_OUT, VALUE_IN } transfer_type_e;

/***********************************************************************************************************
 * Transfer Data data structure
 ***********************************************************************************************************/
typedef struct _transfer_data {
  const flex_trit_t *data;
  uint32_t len;  // length of trits
} transfer_data_t;

/***********************************************************************************************************
 * Transfer Value Out data structure
 ***********************************************************************************************************/
typedef struct _transfer_value_out {
  const flex_trit_t *seed;
  uint8_t security;
  uint64_t seed_index;
} transfer_value_out_t;

/***********************************************************************************************************
 * Transfer Value In data structure
 ***********************************************************************************************************/
typedef struct _transfer_value_in {
  // can have up to 2187 trytes as data
  uint32_t len;  // length of trits
  const flex_trit_t *data;
} transfer_value_in_t;

/***********************************************************************************************************
 * Transfer data structure
 ***********************************************************************************************************/

typedef struct _transfer {
  transfer_type_e type;
  const flex_trit_t *address;
  const flex_trit_t *tag;
  flex_trit_t obsolete_tag[FLEX_TRIT_SIZE_81];
  int64_t value;
  uint64_t timestamp;
  void *meta;  // extend informations depended on transfer type.
} transfer_t;

// create a data transfer object.
transfer_t *transfer_data_new(flex_trit_t const *const address, flex_trit_t const *const tag,
                              flex_trit_t const *const data, uint32_t data_len, uint64_t timestamp);

// create a value_out transfer object.
transfer_t *transfer_value_out_new(transfer_value_out_t const *const output, flex_trit_t const *const tag,
                                   flex_trit_t const *const address, int64_t value, uint64_t timestamp);

// create a value_int transfer object.
transfer_t *transfer_value_in_new(flex_trit_t const *const address, flex_trit_t const *const tag, int64_t value,
                                  flex_trit_t const *const data, uint32_t data_len, uint64_t timestamp);

// Get the number of transactions for this transfer
uint32_t transfer_transactions_count(transfer_t *tf);

// Get transfer value
int64_t transfer_value(transfer_t *tf);
void transfer_free(transfer_t **transfer);

/***********************************************************************************************************
 * Transfer Context data structure
 ***********************************************************************************************************/
typedef struct _transfer_ctx {
  uint32_t count;  // number of transactions
  flex_trit_t bundle[FLEX_TRIT_SIZE_243];
} transfer_ctx_t;

// Creates and returns a new transfer context
bool transfer_ctx_init(transfer_ctx_t *transfer_ctx, transfer_t *transfers[], uint32_t len);

// Calculates the bundle hash for a collection of transfers
void transfer_ctx_hash(transfer_ctx_t *transfer_ctx, Kerl *kerl, transfer_t *transfers[], uint32_t tx_len);

// Returns the resulting bundle hash
// flex_trit_t *transfer_ctx_finalize(transfer_ctx_t *transfer_ctx);

void absorb_essence(Kerl *const kerl, flex_trit_t const *address, int64_t value, flex_trit_t const *obsolete_tag,
                    uint64_t timestamp, int64_t current_index, int64_t last_index, trit_t *const essence_trits);
/***********************************************************************************************************
 * Transfer Iterator data structure
 ***********************************************************************************************************/
typedef flex_trit_t *(*iota_signature_generator)(const flex_trit_t *seed, const size_t index, const size_t security,
                                                 const flex_trit_t *bundleHash);

typedef struct _transfer_iterator {
  uint32_t transfers_count;
  uint32_t transactions_count;
  uint32_t current_transfer;
  uint32_t current_transfer_transaction_index;
  uint32_t current_transaction_index;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  flex_trit_t *transaction_signature;
  iota_transaction_t *transaction;
  uint8_t dynamic_transaction;
  iota_signature_generator iota_signature_gen;
  transfer_t **transfers;
} transfer_iterator_t;

// Returns the next transaction
iota_transaction_t *transfer_iterator_next(transfer_iterator_t *transfer_iterator);

// Creates and returns a new transfer iterator
// if `transaction` is NULL will be dynamically allocated a transaction object.
transfer_iterator_t *transfer_iterator_new(transfer_t *transfers[], uint32_t len, Kerl *kerl,
                                           iota_transaction_t *transaction);

// Free an existing transfer iterator
void transfer_iterator_free(transfer_iterator_t **iter);

bool validate_output(transfer_value_out_t const *const output);

void transfer_logger_init();
void transfer_logger_destroy();

#endif  // __COMMON_MODEL_TRANSFER_H_
#ifdef __cplusplus
}
#endif
