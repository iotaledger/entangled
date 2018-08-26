/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/transfer.h"
#include <string.h>
#include "common/helpers/sign.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/tryte_long.h"

#define TRITS_PER_ESSENCE 486
#define TRITS_PER_BUNDLE_HASH 243
#define TRYTES_PER_ESSENCE 162
#define TRYTES_PER_BUNDLE_HASH 81

/***********************************************************************************************************
 * Transfer Input data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _transfer_output {...};

// Get the seed
const flex_trit_t *transfer_output_seed(transfer_output_t transfer_output) {
  return transfer_output->seed;
}

// Set the seed (assign)
void transfer_output_set_seed(transfer_output_t transfer_output,
                              const flex_trit_t *seed) {
  transfer_output->seed = seed;
}

// Get the security level
size_t transfer_output_security(transfer_output_t transfer_output) {
  return transfer_output->security;
}

// Set the security level
void transfer_output_set_security(transfer_output_t transfer_output,
                                  size_t security) {
  transfer_output->security = security;
}

// Get the index
size_t transfer_output_seed_index(transfer_output_t transfer_output) {
  return transfer_output->seed_index;
}

// Set the index
void transfer_output_set_seed_index(transfer_output_t transfer_output,
                                    size_t index) {
  transfer_output->seed_index = index;
}

// Creates and returns a new transfer output
transfer_output_t transfer_output_new(void) {
  transfer_output_t transfer_output;
  transfer_output = (transfer_output_t)malloc(sizeof(struct _transfer_output));
  if (!transfer_output) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_output, 0, sizeof(struct _transfer_output));
  return transfer_output;
}

// Free an existing transfer output
void transfer_output_free(transfer_output_t transfer_output) {
  free(transfer_output);
}

/***********************************************************************************************************
 * Transfer Data data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _transfer_data {...};

// Get the address
const flex_trit_t *transfer_data_address(transfer_data_t transfer_data) {
  return transfer_data->address;
}

// Set the address (assign)
void transfer_data_set_address(transfer_data_t transfer_data,
                               const flex_trit_t *address) {
  transfer_data->address = address;
}

// Get the length of the data in trits
size_t transfer_data_len(transfer_data_t transfer_data) {
  return transfer_data->len;
}

// Get the data
const flex_trit_t *transfer_data_data(transfer_data_t transfer_data) {
  return transfer_data->data;
}

// Set the data (assign)
void transfer_data_set_data(transfer_data_t transfer_data, flex_trit_t *data,
                            size_t len) {
  transfer_data->data = data;
  transfer_data->len = len;
}

// Get the number of transactions for this data transfer
size_t transfer_data_transactions_count(transfer_data_t transfer_data) {
  return (transfer_data_len(transfer_data) + NUM_TRITS_SIGNATURE - 1) /
         NUM_TRITS_SIGNATURE;
}

// Creates and returns a new transfer data
transfer_data_t transfer_data_new(void) {
  transfer_data_t transfer_data;
  transfer_data = (transfer_data_t)malloc(sizeof(struct _transfer_data));
  if (!transfer_data) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_data, 0, sizeof(struct _transfer_data));
  return transfer_data;
}

// Free an existing transfer data
void transfer_data_free(transfer_data_t transfer_data) { free(transfer_data); }

/***********************************************************************************************************
 * Transfer Value Out data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _transfer_value_out {...};

// Get the address
const flex_trit_t *transfer_value_out_address(
    transfer_value_out_t transfer_value_out) {
  return transfer_value_out->address;
}

// Set the address (assign)
void transfer_value_out_set_address(transfer_value_out_t transfer_value_out,
                                    const flex_trit_t *address) {
  transfer_value_out->address = address;
}

// Get the value
int64_t transfer_value_out_value(transfer_value_out_t transfer_value_out) {
  return transfer_value_out->value;
}

// Set the value
void transfer_value_out_set_value(transfer_value_out_t transfer_value_out,
                                  int64_t value) {
  transfer_value_out->value = value;
}

// Get the transfer output
transfer_output_t transfer_value_out_output(
    transfer_value_out_t transfer_value_out) {
  return transfer_value_out->output;
}

// Set the transfer output (assign)
void transfer_value_out_set_output(transfer_value_out_t transfer_value_out,
                                   transfer_output_t transfer_output) {
  transfer_value_out->output = transfer_output;
}

// Get the number of transactions for this value out transfer
size_t transfer_value_out_transactions_count(
    transfer_value_out_t transfer_value_out) {
  return transfer_output_security(
      transfer_value_out_output(transfer_value_out));
}

// Creates and returns a new transfer value out
transfer_value_out_t transfer_value_out_new(void) {
  transfer_value_out_t transfer_value_out;
  transfer_value_out =
      (transfer_value_out_t)malloc(sizeof(struct _transfer_value_out));
  if (!transfer_value_out) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_value_out, 0, sizeof(struct _transfer_value_out));
  return transfer_value_out;
}

// Free an existing transfer value out
void transfer_value_out_free(transfer_value_out_t transfer_value_out) {
  free(transfer_value_out);
}

/***********************************************************************************************************
 * Transfer Value In data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _transfer_value_in {...};

// Get the address
const flex_trit_t *transfer_value_in_address(
    transfer_value_in_t transfer_value_in) {
  return transfer_value_in->address;
}

// Set the address (assign)
void transfer_value_in_set_address(transfer_value_in_t transfer_value_in,
                                   const flex_trit_t *address) {
  transfer_value_in->address = address;
}

// Get the value
int64_t transfer_value_in_value(transfer_value_in_t transfer_value_in) {
  return transfer_value_in->value;
}

// Set the value
void transfer_value_in_set_value(transfer_value_in_t transfer_value_in,
                                 int64_t value) {
  transfer_value_in->value = value;
}

// Get the length of the data
size_t transfer_value_in_len(transfer_value_in_t transfer_value_in) {
  return transfer_value_in->len;
}

// Get the data
const flex_trit_t *transfer_value_in_data(
    transfer_value_in_t transfer_value_in) {
  return transfer_value_in->data;
}

// Set the data (assign)
void transfer_value_in_set_data(transfer_value_in_t transfer_value_in,
                                flex_trit_t *data, size_t len) {
  transfer_value_in->len = len;
  transfer_value_in->data = data;
}

// Get the number of transactions for this value in transfer
size_t transfer_value_in_transactions_count(
    transfer_value_in_t transfer_value_in) {
  return 1;
}

// Creates and returns a new transfer value in
transfer_value_in_t transfer_value_in_new(void) {
  transfer_value_in_t transfer_value_in;
  transfer_value_in =
      (transfer_value_in_t)malloc(sizeof(struct _transfer_value_in));
  if (!transfer_value_in) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_value_in, 0, sizeof(struct _transfer_value_in));
  return transfer_value_in;
}

// Free an existing transfer value in
void transfer_value_in_free(transfer_value_in_t transfer_value_in) {
  free(transfer_value_in);
}

/***********************************************************************************************************
 * Transfer data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _transfer {...};

// Get the type
transfer_type_e transfer_type(transfer_t transfer) { return transfer->type; }

// Get the number of transactions for this transfer
size_t transfer_transactions_count(transfer_t transfer) {
  switch (transfer_type(transfer)) {
    case DATA:
      return transfer_data_transactions_count(transfer_data(transfer));
    case VALUE_OUT:
      return transfer_value_out_transactions_count(
          transfer_value_out(transfer));
    case VALUE_IN:
      return transfer_value_in_transactions_count(transfer_value_in(transfer));
  }
  // Should not reach here
  return 0;
}

// Get the tag
const flex_trit_t *transfer_tag(transfer_t transfer) { return transfer->tag; }

// Set the tag (assign)
void transfer_set_tag(transfer_t transfer, flex_trit_t *tag) {
  transfer->tag = tag;
}

// Get the timestamp
uint64_t transfer_timestamp(transfer_t transfer) { return transfer->timestamp; }

// Set the timestamp
void transfer_set_timestamp(transfer_t transfer, uint64_t timestamp) {
  transfer->timestamp = timestamp;
}

// Get the transfer value
int64_t transfer_value(transfer_t transfer) {
  switch (transfer_type(transfer)) {
    case DATA:
      return 0;
    case VALUE_OUT:
      return transfer_value_out_value(transfer_value_out(transfer));
    case VALUE_IN:
      return transfer_value_in_value(transfer_value_in(transfer));
  }
  // Should not reach here
  return 0;
}

// Get the transfer value
const flex_trit_t *transfer_address(transfer_t transfer) {
  switch (transfer_type(transfer)) {
    case DATA:
      return transfer_data_address(transfer_data(transfer));
    case VALUE_OUT:
      return transfer_value_out_address(transfer_value_out(transfer));
    case VALUE_IN:
      return transfer_value_in_address(transfer_value_in(transfer));
  }
  // Should not reach here
  return NULL;
}

// Get the transfer data
transfer_data_t transfer_data(transfer_t transfer) {
  if (transfer_type(transfer) != DATA) {
    return NULL;
  }
  return transfer->data;
}

// Get the transfer value in
transfer_value_out_t transfer_value_out(transfer_t transfer) {
  if (transfer_type(transfer) != VALUE_OUT) {
    return NULL;
  }
  return transfer->in;
}

// Get the transfer value out
transfer_value_in_t transfer_value_in(transfer_t transfer) {
  if (transfer_type(transfer) != VALUE_IN) {
    return NULL;
  }
  return transfer->out;
}

// Creates and returns a new transfer
transfer_t transfer_new(transfer_type_e transfer_type) {
  transfer_t transfer;
  transfer = (transfer_t)malloc(sizeof(struct _transfer));
  if (!transfer) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer, 0, sizeof(struct _transfer));
  transfer->type = transfer_type;
  switch (transfer_type) {
    case DATA:
      transfer->data = transfer_data_new();
      break;
    case VALUE_OUT:
      transfer->in = transfer_value_out_new();
      break;
    case VALUE_IN:
      transfer->out = transfer_value_in_new();
      break;
    default:
      free(transfer);
      transfer = NULL;
  }
  return transfer;
}

// Free an existing transfer
void transfer_free(transfer_t transfer) {
  switch (transfer_type(transfer)) {
    case DATA:
      transfer_data_free(transfer_data(transfer));
      break;
    case VALUE_OUT:
      transfer_value_out_free(transfer_value_out(transfer));
      break;
    case VALUE_IN:
      transfer_value_in_free(transfer_value_in(transfer));
      break;
  }
  free(transfer);
}

/***********************************************************************************************************
 * Transfer Context data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _transfer_ctx {...};

// Creates and returns a new transfer context
int transfer_ctx_init(transfer_ctx_t transfer_ctx, transfer_t *transfers,
                      size_t len) {
  size_t i;
  int64_t total = 0;
  for (i = 0; i < len; i++) {
    transfer_t transfer = transfers[i];
    transfer_ctx->count += transfer_transactions_count(transfer);
    total += transfer_value(transfer);
  }
  return total ? -1 : 0;
}

// Gets the count of transactions for the transfers
int transfer_ctx_count(transfer_ctx_t transfer_ctx) {
  return transfer_ctx->count;
}

// Calculates the bundle hash for a collection of transfers
int transfer_ctx_hash(transfer_ctx_t transfer_ctx, Kerl *kerl,
                      transfer_t *transfers, size_t len) {
  size_t i, j, count, current_index = 0;
  tryte_t essence[TRYTES_PER_ESSENCE];
  trit_t essence_trits[TRITS_PER_ESSENCE];

  // Calculate bundle hash
  for (i = 0; i < len; i++) {
    transfer_t transfer = transfers[i];
    count = transfer_transactions_count(transfer);
    for (j = 0; j < count; j++) {
      // Set essence trytes to 9
      memset(essence, '9', TRYTES_PER_ESSENCE);
      // essence = Address + Value + Tag + Timestamp + Current Index + Last
      // Index
      flex_trits_to_trytes(essence, NUM_TRYTES_ADDRESS,
                           transfer_address(transfer), NUM_TRITS_ADDRESS,
                           NUM_TRITS_ADDRESS);
      int64_t value = transfer_type(transfer) == VALUE_OUT
                          ? j == 0 ? transfer_value(transfer) : 0
                          : 0;
      long_to_trytes(value, &essence[81]);
      flex_trits_to_trytes(&essence[108], NUM_TRYTES_TAG, transfer_tag(transfer),
                           NUM_TRITS_TAG, NUM_TRITS_TAG);
      long_to_trytes(transfer_timestamp(transfer), essence + 135);
      long_to_trytes(current_index, essence + 144);
      long_to_trytes(transfer_ctx->count - 1, essence + 153);
      // essence in in trytes, convert to trits
      trytes_to_trits(essence, essence_trits, TRYTES_PER_ESSENCE);
      // Absorb essence in kerl
      kerl_absorb(kerl, essence_trits, TRITS_PER_ESSENCE);
      current_index++;
    }
  }
  trit_t bundle_trit[TRITS_PER_BUNDLE_HASH];
  // Squeeze kerl to get the bundle hash
  kerl_squeeze(kerl, bundle_trit, TRITS_PER_BUNDLE_HASH);
  flex_trits_from_trits(transfer_ctx->bundle, TRITS_PER_BUNDLE_HASH,
                        bundle_trit, TRITS_PER_BUNDLE_HASH,
                        TRITS_PER_BUNDLE_HASH);
  return 0;
}

// Returns the resulting bundle hash
flex_trit_t *transfer_ctx_finalize(transfer_ctx_t transfer_ctx) {
  return transfer_ctx->bundle;
}

// Creates and returns a new transfer context
transfer_ctx_t transfer_ctx_new(void) {
  transfer_ctx_t transfer_ctx;
  transfer_ctx = (transfer_ctx_t)malloc(sizeof(struct _transfer_ctx));
  if (!transfer_ctx) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_ctx, 0, sizeof(struct _transfer_ctx));
  memset(transfer_ctx->bundle, FLEX_TRIT_NULL_VALUE,
         sizeof(transfer_ctx->bundle));
  return transfer_ctx;
}

// Free an existing transfer context
void transfer_ctx_free(transfer_ctx_t transfer_ctx) { free(transfer_ctx); }

/***********************************************************************************************************
 * Transfer Iterator data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _transfer_iterator {...};

/***********************************************************************************************************
 * Private interface
 ***********************************************************************************************************/

void transfer_iterator_next_data_transaction(
    transfer_iterator_t transfer_iterator, transfer_t transfer) {
  transfer_data_t trans_data = transfer_data(transfer);
  const flex_trit_t *data = transfer_data_data(trans_data);
  // Length of message in trits
  size_t data_len = transfer_data_len(trans_data);
  // Offset of message data for current index
  size_t offset = transfer_iterator->current_transfer_transaction_index *
                  NUM_TRITS_SIGNATURE;
  // Length of the message data for the current transaction
  size_t len = data_len - offset;
  len = len > NUM_TRITS_SIGNATURE ? NUM_TRITS_SIGNATURE : len;
  size_t flex_size = num_flex_trits_for_trits(len);
  flex_trit_t *trits = transaction_message(transfer_iterator->transaction);
  memset(trits, FLEX_TRIT_NULL_VALUE, flex_size);
  flex_trits_slice(trits, flex_size, data, data_len, offset, len);
}

void transfer_iterator_next_output_transaction(
    transfer_iterator_t transfer_iterator, transfer_t transfer) {
  transfer_value_out_t value_out = transfer_value_out(transfer);
  transfer_output_t output = transfer_value_out_output(value_out);
  if (transfer_iterator->current_transfer_transaction_index == 0) {
    if (transfer_iterator->transaction_signature) {
      free(transfer_iterator->transaction_signature);
    }
    iota_signature_generator iota_signature_gen =
        transfer_iterator_sign_gen(transfer_iterator);
    transfer_iterator->transaction_signature =
        (flex_trit_t *)iota_signature_gen(
            transfer_output_seed(output), transfer_output_seed_index(output),
            transfer_output_security(output), transfer_iterator->bundle_hash);
    transaction_set_value(transfer_iterator->transaction,
                          transfer_value(transfer));
  }
  flex_trits_slice(
      transaction_signature(transfer_iterator->transaction),
      NUM_TRITS_SIGNATURE, transfer_iterator->transaction_signature,
      NUM_TRITS_SIGNATURE * transfer_output_security(output),
      NUM_TRITS_SIGNATURE *
          transfer_iterator->current_transfer_transaction_index,
      NUM_TRITS_SIGNATURE);
}

void transfer_iterator_next_input_transaction(
    transfer_iterator_t transfer_iterator, transfer_t transfer) {
  transfer_value_in_t value_in = transfer_value_in(transfer);
  transaction_set_message(transfer_iterator->transaction,
                          transfer_value_in_data(value_in));
  transaction_set_value(transfer_iterator->transaction,
                        transfer_value(transfer));
}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/
// Returns the next transaction
iota_transaction_t transfer_iterator_next(
    transfer_iterator_t transfer_iterator) {
  iota_transaction_t transaction = NULL;
  if (transfer_iterator->current_transfer <
      transfer_iterator->transfers_count) {
    // Dynamically allocate a transaction structure if none was
    // previously provided with transfer_iterator_set_transaction
    if (!transfer_iterator->transaction) {
      transfer_iterator->transaction = transaction_new();
      transfer_iterator->dynamic_transaction = 1;
    }
    transfer_t transfer =
        transfer_iterator->transfers[transfer_iterator->current_transfer];
    size_t count = transfer_transactions_count(transfer);
    if (transfer_iterator->current_transfer_transaction_index >= count) {
      transfer_iterator->current_transfer++;
      transfer_iterator->current_transfer_transaction_index = 0;
      return transfer_iterator_next(transfer_iterator);
    }
    // Reset all transaction fields
    transaction_reset(transfer_iterator->transaction);
    // Set common transaction fields
    transaction_set_bundle(transfer_iterator->transaction,
                           transfer_iterator->bundle_hash);
    transaction_set_address(transfer_iterator->transaction,
                            transfer_address(transfer));
    transaction_set_obsolete_tag(transfer_iterator->transaction,
                                 transfer_tag(transfer));
    transaction_set_tag(transfer_iterator->transaction, transfer_tag(transfer));
    transaction_set_timestamp(transfer_iterator->transaction,
                              transfer_timestamp(transfer));
    transaction_set_current_index(transfer_iterator->transaction,
                                  transfer_iterator->current_transaction_index);
    transaction_set_last_index(transfer_iterator->transaction,
                               transfer_iterator->transactions_count - 1);
    // Set transaction type specific fields
    switch (transfer_type(transfer)) {
      case DATA:
        transfer_iterator_next_data_transaction(transfer_iterator, transfer);
        break;
      case VALUE_OUT:
        transfer_iterator_next_output_transaction(transfer_iterator, transfer);
        break;
      case VALUE_IN:
        transfer_iterator_next_input_transaction(transfer_iterator, transfer);
        break;
    }
    transaction = transfer_iterator->transaction;
    transfer_iterator->current_transfer_transaction_index++;
    transfer_iterator->current_transaction_index++;
  }
  return transaction;
}

// Creates and returns a new transfer iterator
transfer_iterator_t transfer_iterator_new(transfer_t *transfers, size_t len,
                                          Kerl *kerl) {
  transfer_iterator_t transfer_iterator;
  transfer_iterator =
      (transfer_iterator_t)malloc(sizeof(struct _transfer_iterator));
  if (!transfer_iterator) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_iterator, 0, sizeof(struct _transfer_iterator));
  transfer_iterator->transfers = transfers;
  transfer_iterator->transfers_count = len;
  transfer_ctx_t transfer_ctx = transfer_ctx_new();
  if (!transfer_ctx) {
    // errno = IOTA_OUT_OF_MEMORY
    transfer_iterator_free(transfer_iterator);
    return NULL;
  }
  transfer_ctx_init(transfer_ctx, transfers, len);
  transfer_iterator->transactions_count = transfer_ctx_count(transfer_ctx);
  transfer_ctx_hash(transfer_ctx, kerl, transfers, len);
  memcpy(transfer_iterator->bundle_hash, transfer_ctx_finalize(transfer_ctx),
         FLEX_TRIT_SIZE_243);
  transfer_ctx_free(transfer_ctx);
  transfer_iterator_set_sign_gen(transfer_iterator,
                                 iota_flex_sign_signature_gen);
  return transfer_iterator;
}

// Set statically allocated transaction - If not used, will be dynamically
// allocated
void transfer_iterator_set_transaction(transfer_iterator_t transfer_iterator,
                                       iota_transaction_t transaction) {
  transfer_iterator->transaction = transaction;
}

// Get signature generator function
iota_signature_generator transfer_iterator_sign_gen(
    transfer_iterator_t transfer_iterator) {
  return transfer_iterator->iota_signature_gen;
}

// Set signature generator function
void transfer_iterator_set_sign_gen(
    transfer_iterator_t transfer_iterator,
    iota_signature_generator iota_signature_gen) {
  transfer_iterator->iota_signature_gen = iota_signature_gen;
}

// Free an existing transfer iterator
void transfer_iterator_free(transfer_iterator_t transfer_iterator) {
  if (transfer_iterator->dynamic_transaction &&
      transfer_iterator->transaction) {
    transaction_free(transfer_iterator->transaction);
  }
  if (transfer_iterator->transaction_signature) {
    free(transfer_iterator->transaction_signature);
  }
  free(transfer_iterator);
}
