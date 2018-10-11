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
#include "common/kerl/kerl.h"
#include "common/model/transaction.h"

#define NUM_TRITS_ESSENCE 486

/***********************************************************************************************************
 * Transfer Input data structure
 ***********************************************************************************************************/
typedef struct _transfer_output *transfer_output_t;
struct _transfer_output {
  const flex_trit_t *seed;
  size_t security;
  size_t seed_index;
};

// Get the seed
const flex_trit_t *transfer_output_seed(transfer_output_t transfer_output);
// Set the seed (assign)
void transfer_output_set_seed(transfer_output_t transfer_output,
                              const flex_trit_t *seed);
// Get the security level
size_t transfer_output_security(transfer_output_t transfer_output);
// Set the security level
void transfer_output_set_security(transfer_output_t transfer_output,
                                  size_t security);
// Get the index
size_t transfer_output_seed_index(transfer_output_t transfer_output);
// Set the index
void transfer_output_set_seed_index(transfer_output_t transfer_output,
                                    size_t index);
// Creates and returns a new transfer input
transfer_output_t transfer_output_new(void);
// Free an existing transfer input
void transfer_output_free(transfer_output_t transfer_output);

/***********************************************************************************************************
 * Transfer Data data structure
 ***********************************************************************************************************/
typedef struct _transfer_data *transfer_data_t;
struct _transfer_data {
  const flex_trit_t *address;
  size_t len;
  const flex_trit_t *data;
};

// Get the address
const flex_trit_t *transfer_data_address(transfer_data_t transfer_data);
// Set the address (assign)
void transfer_data_set_address(transfer_data_t transfer_data,
                               const flex_trit_t *address);
// Get the length of the data
size_t transfer_data_len(transfer_data_t transfer_data);
// Get the data
const flex_trit_t *transfer_data_data(transfer_data_t transfer_data);
// Set the data (assign), len is number of trits
void transfer_data_set_data(transfer_data_t transfer_data, flex_trit_t *data,
                            size_t len);
// Get the number of transactions for this data transfer
size_t transfer_data_transactions_count(transfer_data_t transfer_data);

/***********************************************************************************************************
 * Transfer Value Out data structure
 ***********************************************************************************************************/
typedef struct _transfer_value_out *transfer_value_out_t;
struct _transfer_value_out {
  const flex_trit_t *address;
  int64_t value;
  transfer_output_t output;
};

// Get the address
const flex_trit_t *transfer_value_out_address(
    transfer_value_out_t transfer_value_out);
// Set the address (assign)
void transfer_value_out_set_address(transfer_value_out_t transfer_value_out,
                                    const flex_trit_t *address);
// Get the value
int64_t transfer_value_out_value(transfer_value_out_t transfer_value_out);
// Set the value
void transfer_value_out_set_value(transfer_value_out_t transfer_value_out,
                                  int64_t value);
// Get the transfer output
transfer_output_t transfer_value_out_output(
    transfer_value_out_t transfer_value_out);
// Set the transfer output (assign)
void transfer_value_out_set_output(transfer_value_out_t transfer_value_out,
                                   transfer_output_t transfer_output);
// Get the number of transactions for this value out transfer
size_t transfer_value_out_transactions_count(
    transfer_value_out_t transfer_value_out);

/***********************************************************************************************************
 * Transfer Value In data structure
 ***********************************************************************************************************/
typedef struct _transfer_value_in *transfer_value_in_t;
struct _transfer_value_in {
  const flex_trit_t *address;
  uint64_t value;
  // can have up to 2187 trytes as data
  size_t len;
  const flex_trit_t *data;
};

// Get the address
const flex_trit_t *transfer_value_in_address(
    transfer_value_in_t transfer_value_in);
// Set the address (assign)
void transfer_value_in_set_address(transfer_value_in_t transfer_value_in,
                                   const flex_trit_t *address);
// Get the value
int64_t transfer_value_in_value(transfer_value_in_t transfer_value_in);
// Set the value
void transfer_value_in_set_value(transfer_value_in_t transfer_value_in,
                                 int64_t value);
// Get the length of the data
size_t transfer_value_in_len(transfer_value_in_t transfer_value_in);
// Get the data
const flex_trit_t *transfer_value_in_data(
    transfer_value_in_t transfer_value_in);
// Set the data (assign)
void transfer_value_in_set_data(transfer_value_in_t transfer_value_in,
                                flex_trit_t *data, size_t len);
// Get the number of transactions for this value int transfer
size_t transfer_value_in_transactions_count(
    transfer_value_in_t transfer_value_in);

/***********************************************************************************************************
 * Transfer data structure
 ***********************************************************************************************************/
typedef enum { DATA = 0, VALUE_OUT, VALUE_IN } transfer_type_e;

typedef struct _transfer *transfer_t;
struct _transfer {
  transfer_type_e type;
  const flex_trit_t *tag;
  uint64_t timestamp;
  union {
    transfer_data_t data;
    transfer_value_out_t in;
    transfer_value_in_t out;
  };
};

// Get the type
transfer_type_e transfer_type(transfer_t transfer);
// Get the number of transactions for this transfer
size_t transfer_transactions_count(transfer_t transfer);
// Get the tag
const flex_trit_t *transfer_tag(transfer_t transfer);
// Set the tag (assign)
void transfer_set_tag(transfer_t transfer, flex_trit_t *tag);
// Get the timestamp
uint64_t transfer_timestamp(transfer_t transfer);
// Set the timestamp
void transfer_set_timestamp(transfer_t transfer, uint64_t timestamp);
// Get the transfer value
int64_t transfer_value(transfer_t transfer);
// Get the transfer value
const flex_trit_t *transfer_address(transfer_t transfer);
// Get the transfer data or NULL if type not DATA
transfer_data_t transfer_data(transfer_t transfer);
// Get the transfer value in or NULL if type not VALUE_OUT
transfer_value_out_t transfer_value_out(transfer_t transfer);
// Get the transfer value out or NULL if type not VALUE_IN
transfer_value_in_t transfer_value_in(transfer_t transfer);
// Creates and returns a new transfer
transfer_t transfer_new(transfer_type_e transfer_type);
// Free an existing transfer
void transfer_free(transfer_t transfer);

/***********************************************************************************************************
 * Transfer Context data structure
 ***********************************************************************************************************/
typedef struct _transfer_ctx *transfer_ctx_t;
struct _transfer_ctx {
  size_t count;
  flex_trit_t bundle[FLEX_TRIT_SIZE_243];
};

// Creates and returns a new transfer context
int transfer_ctx_init(transfer_ctx_t transfer_ctx, transfer_t *transfers,
                      size_t len);
// Gets the count of transactions for the transfers
int transfer_ctx_count(transfer_ctx_t transfer_ctx);
// Calculates the bundle hash for a collection of transfers
int transfer_ctx_hash(transfer_ctx_t transfer_ctx, Kerl *kerl,
                      transfer_t *transfers, size_t len);
// Returns the resulting bundle hash
flex_trit_t *transfer_ctx_finalize(transfer_ctx_t transfer_ctx);
// Creates and returns a new transfer context
transfer_ctx_t transfer_ctx_new(void);
// Free an existing transfer context
void transfer_ctx_free(transfer_ctx_t transfer_ctx);

void absorb_essence(Kerl *const kerl, flex_trit_t const *address, int64_t value,
                    flex_trit_t const *obsolete_tag, uint64_t timestamp,
                    int64_t current_index, int64_t last_index,
                    trit_t *const essence_trits);
/***********************************************************************************************************
 * Transfer Iterator data structure
 ***********************************************************************************************************/
typedef flex_trit_t *(*iota_signature_generator)(const flex_trit_t *seed,
                                                 const size_t index,
                                                 const size_t security,
                                                 const flex_trit_t *bundleHash);

typedef struct _transfer_iterator *transfer_iterator_t;
struct _transfer_iterator {
  transfer_t *transfers;
  size_t transfers_count;
  size_t transactions_count;
  size_t current_transfer;
  size_t current_transfer_transaction_index;
  size_t current_transaction_index;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  flex_trit_t *transaction_signature;
  iota_transaction_t transaction;
  uint8_t dynamic_transaction;
  iota_signature_generator iota_signature_gen;
};

// Returns the next transaction
iota_transaction_t transfer_iterator_next(
    transfer_iterator_t transfer_iterator);
// Creates and returns a new transfer iterator
transfer_iterator_t transfer_iterator_new(transfer_t *transfers, size_t len,
                                          Kerl *kerl);
// Set statically allocated transaction - If not used, will be dynamically
// allocated
void transfer_iterator_set_transaction(transfer_iterator_t transfer_iterator,
                                       iota_transaction_t transaction);
// Get signature generator function
iota_signature_generator transfer_iterator_sign_gen(
    transfer_iterator_t transfer_iterator);
// Set signature generator function
void transfer_iterator_set_sign_gen(
    transfer_iterator_t transfer_iterator,
    iota_signature_generator iota_signature_gen);
// Free an existing transfer iterator
void transfer_iterator_free(transfer_iterator_t transfer_iterator);

#endif  // __COMMON_MODEL_TRANSFER_H_
#ifdef __cplusplus
}
#endif
