/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
#include "common/model/transfer.h"
#include "common/trinary/tryte_long.h"
#include "common/trinary/trit_tryte.h"
#include "common/helpers/sign.h"

#define TRYTES_PER_MESSAGE 2187

void _convert_and_pad(int64_t value, tryte_t *trytes, size_t length) {
  size_t num_trytes;
  num_trytes = long_to_trytes(value, trytes);
  memset(trytes + num_trytes, '9', length - num_trytes);
}

/***********************************************************************************************************
 * Transfer Input data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer_output {...};

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

// Get the seed
tryte_t *transfer_output_seed(transfer_output_t transfer_output) {
  return transfer_output->seed;
}

// Set the seed (copy argument)
void transfer_output_set_seed(transfer_output_t transfer_output, const tryte_t *seed) {
  memcpy(transfer_output->seed, seed, sizeof(transfer_output->seed));
}

// Get the security level
size_t transfer_output_security(transfer_output_t transfer_output) {
  return transfer_output->security;
}

// Set the security level
void transfer_output_set_security(transfer_output_t transfer_output, size_t security) {
  transfer_output->security = security;
}

// Get the index
size_t transfer_output_index(transfer_output_t transfer_output) {
  return transfer_output->index;
}

// Set the index
void transfer_output_set_index(transfer_output_t transfer_output, size_t index) {
  transfer_output->index = index;
}

// Free an existing transfer output - compatible with free()
void transfer_output_free(void *t) {
  if (t) {
    free(t);
  }
}

/***********************************************************************************************************
 * Transfer Data data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer_data {...};

// Get the address
tryte_t *transfer_data_address(transfer_data_t transfer_data) {
  return transfer_data->address;
}

// Set the address (copy argument)
void transfer_data_set_address(transfer_data_t transfer_data, const tryte_t *address) {
  memcpy(transfer_data->address, address, sizeof(transfer_data->address));
}

// Get the length of the data
size_t transfer_data_len(transfer_data_t transfer_data) {
  return transfer_data->len;
}

// Get the data
tryte_t *transfer_data_data(transfer_data_t transfer_data) {
  return transfer_data->data;
}

// Set the data (copy argument)
void transfer_data_set_data(transfer_data_t transfer_data, tryte_t *data, size_t len) {
  if (transfer_data->data) {
    free(transfer_data->data);
    transfer_data->data = NULL;
  }
  size_t data_len = len * sizeof(tryte_t);
  transfer_data->data = (tryte_t *)malloc(data_len);
  if (transfer_data->data) {
    memcpy(transfer_data->data, data, data_len);
  }
}

// Get the number of transactions for this data transfer
size_t transfer_data_transactions_count(transfer_data_t transfer_data) {
  return (transfer_data_len(transfer_data) + TRYTES_PER_MESSAGE - 1) / TRYTES_PER_MESSAGE;
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

// Free an existing transfer data - compatible with free()
void transfer_data_free(void *t) {
  if (t) {
    transfer_data_t transfer_data = (transfer_data_t)t;
    if (transfer_data->data) {
      free(transfer_data->data);
    }
    free(t);
  }
}

/***********************************************************************************************************
 * Transfer Value Out data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer_value_out {...};

// Get the address
tryte_t *transfer_value_out_address(transfer_value_out_t transfer_value_out) {
  return transfer_value_out->address;
}

// Set the address (copy argument)
void transfer_value_out_set_address(transfer_value_out_t transfer_value_out, const tryte_t *address) {
  memcpy(transfer_value_out->address, address, sizeof(transfer_value_out->address));
}

// Get the value
int64_t transfer_value_out_value(transfer_value_out_t transfer_value_out) {
  return transfer_value_out->value;
}

// Set the value
void transfer_value_out_set_value(transfer_value_out_t transfer_value_out, int64_t value) {
  transfer_value_out->value = value;
}

// Get the transfer output
transfer_output_t transfer_value_out_output(transfer_value_out_t transfer_value_out) {
  return transfer_value_out->output;
}

// Set the transfer output (copy argument)
void transfer_value_out_set_output(transfer_value_out_t transfer_value_out, transfer_output_t transfer_output) {
  if (transfer_value_out->output) {
    transfer_value_out_free(transfer_value_out->output);
    transfer_value_out->output = NULL;
  }
  transfer_output_t output = transfer_output_new();
  transfer_output_set_seed(output, transfer_output_seed(transfer_output));
  transfer_output_set_security(output, transfer_output_security(transfer_output));
  transfer_output_set_index(output, transfer_output_index(transfer_output));
  transfer_value_out->output = transfer_output;
}

// Get the number of transactions for this value in transfer
size_t transfer_value_out_transactions_count(transfer_value_out_t transfer_value_out) {
  return transfer_output_security(transfer_value_out_output(transfer_value_out));
}

// Creates and returns a new transfer value in
transfer_value_out_t transfer_value_out_new(void) {
  transfer_value_out_t transfer_value_out;
  transfer_value_out = (transfer_value_out_t)malloc(sizeof(struct _transfer_value_out));
  if (!transfer_value_out) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_value_out, 0, sizeof(struct _transfer_value_out));
  transfer_value_out->output = transfer_output_new();
  if (!transfer_value_out->output) {
    // errno = IOTA_OUT_OF_MEMORY
    transfer_value_out_free(transfer_value_out);
    transfer_value_out = NULL;
  }
  return transfer_value_out;
}

// Free an existing transfer value in - compatible with free()
void transfer_value_out_free(void *t) {
  if (t) {
    transfer_value_out_t transfer_value_out = (transfer_value_out_t)t;
    if (transfer_value_out->output) {
      transfer_output_free(transfer_value_out->output);
    }
    free(t);
  }
}

/***********************************************************************************************************
 * Transfer Value In data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer_value_in {...};

// Get the address
tryte_t *transfer_value_in_address(transfer_value_in_t transfer_value_in) {
  return transfer_value_in->address;
}

// Set the address (copy argument)
void transfer_value_in_set_address(transfer_value_in_t transfer_value_in, const tryte_t *address) {
  memcpy(transfer_value_in->address, address, sizeof(transfer_value_in->address));
}

// Get the value
int64_t transfer_value_in_value(transfer_value_in_t transfer_value_in) {
  return transfer_value_in->value;
}

// Set the value
void transfer_value_in_set_value(transfer_value_in_t transfer_value_in, int64_t value) {
  transfer_value_in->value = value;
}

// Get the length of the data
size_t transfer_value_in_len(transfer_value_in_t transfer_value_in) {
  return transfer_value_in->len;
}

// Get the data
tryte_t *transfer_value_in_data(transfer_value_in_t transfer_value_in) {
  return transfer_value_in->data;
}

// Set the data (copy argument)
void transfer_value_in_set_data(transfer_value_in_t transfer_value_in, tryte_t *data, size_t len) {
  if (transfer_value_in->data) {
    free(transfer_value_in->data);
    transfer_value_in->data = NULL;
  }
  size_t data_len = len * sizeof(tryte_t);
  transfer_value_in->data = (tryte_t *)malloc(data_len);
  if (transfer_value_in->data) {
    memcpy(transfer_value_in->data, data, data_len);
  }  
}

// Get the number of transactions for this value out transfer
size_t transfer_value_in_transactions_count(transfer_value_in_t transfer_value_in) {
  return 1;
}

// Creates and returns a new transfer value out
transfer_value_in_t transfer_value_in_new(void) {
  transfer_value_in_t transfer_value_in;
  transfer_value_in = (transfer_value_in_t)malloc(sizeof(struct _transfer_value_in));
  if (!transfer_value_in) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_value_in, 0, sizeof(struct _transfer_value_in));
  return transfer_value_in;
}

// Free an existing transfer value out - compatible with free()
void transfer_value_in_free(void *t) {
  if (t) {
    transfer_value_in_t transfer_value_in = (transfer_value_in_t)t;
    if (transfer_value_in->data) {
      free(transfer_value_in->data);
    }
    free(t);
  }  
}

/***********************************************************************************************************
 * Transfer data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer {...};

// Get the type
transfer_type_e transfer_type(transfer_t transfer) {
  return transfer->type;
}

// Get the number of transactions for this transfer
size_t transfer_transactions_count(transfer_t transfer) {
  switch (transfer_type(transfer)) {
    case DATA:
      return transfer_data_transactions_count(transfer_data(transfer));
    case VALUE_OUT:
      return transfer_value_out_transactions_count(transfer_value_out(transfer));
    case VALUE_IN:
      return transfer_value_in_transactions_count(transfer_value_in(transfer));
  }
  // Should not reach here
  return 0;
}

// Get the tag
tryte_t *transfer_tag(transfer_t transfer) {
  return transfer->tag;
}

// Set the tag (copy argument)
void transfer_set_tag(transfer_t transfer, tryte_t *tag) {
  memcpy(transfer->tag, tag, sizeof(transfer->tag));  
}

// Get the timestamp
uint64_t transfer_timestamp(transfer_t transfer) {
  return transfer->timestamp;
}

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
tryte_t *transfer_address(transfer_t transfer) {
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

// Free an existing transfer - compatible with free()
void transfer_free(void *t) {
  if (t) {
    transfer_t transfer = (transfer_t)t;
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
    free(t);
  }
}

/***********************************************************************************************************
 * Transfer Context data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer_ctx {...};

// Creates and returns a new transfer context
int transfer_ctx_init(transfer_ctx_t transfer_ctx, transfer_t *transfers, size_t len) {
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
int transfer_ctx_hash(transfer_ctx_t transfer_ctx, Kerl* kerl, transfer_t *transfers, size_t len) {
  size_t i, j, current_index, count;
  current_index = 0;

  // Calculate bundle hash
  for (i = 0; i < len; i++) {
    tryte_t essence[162];
    transfer_t transfer = transfers[i];
    count = transfer_transactions_count(transfer);
    for (j = 0; j < count; j++) {
      // essence = Address + Value + Tag + Timestamp + Current Index + Last Index
      tryte_t value[27];
      int64_t effective_value = transfer_type(transfer) == VALUE_OUT ? j == 0 ? transfer_value(transfer) : 0 : 0;
      _convert_and_pad(effective_value, value, 27);
      tryte_t timestamp[9];
      _convert_and_pad(transfer_timestamp(transfer), timestamp, 9);
      tryte_t current_index[9];
      _convert_and_pad((int64_t)index, current_index, 9);
      tryte_t last_index[9];
      _convert_and_pad(transfer_ctx->count, last_index, 9);
      memcpy(essence + 0, transfer_address(transfer), 81);
      memcpy(essence + 81, value, 27);
      memcpy(essence + 108, transfer_tag(transfer), 27);
      memcpy(essence + 135, timestamp, 9);
      memcpy(essence + 144, current_index, 9);
      memcpy(essence + 153, last_index, 9);
      // essence in in trytes, convert to trits
      trit_t essence_trit[468];
      trytes_to_trits(essence, essence_trit, 162);
      kerl_absorb(kerl, essence_trit, 468);
    }
  }
  trit_t bundle_trit[243];
  kerl_squeeze(kerl, bundle_trit, 243);
  trits_to_trytes(bundle_trit, transfer_ctx->bundle, 243);
  return 0;
}

// Returns the resulting bundle hash
tryte_t *transfer_ctx_finalize(transfer_ctx_t transfer_ctx) {
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
  return transfer_ctx;
}

// Free an existing transfer context - compatible with free()
void transfer_ctx_free(void *t) {
  if (t) {
    free(t);
  }
}

/***********************************************************************************************************
 * Transfer Iterator data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer_iterator {...};

/***********************************************************************************************************
 * Private interface
 ***********************************************************************************************************/

iota_transaction_t transfer_iterator_next_data_transaction(transfer_iterator_t transfer_iterator, const tryte_t *seed) {
  transfer_t transfer = transfer_iterator->transfers[transfer_iterator->current_transfer];
  size_t count = transfer_transactions_count(transfer);
  if (transfer_iterator->current_transfer_transaction_index >= count) {
    transfer_iterator->current_transfer_transaction_index = 0;
    return NULL;
  }
  if (transfer_iterator->current_transfer_transaction_index == 0) {
    // iota_sign_signature_gen((const char *)seed, const size_t index,
    //                         const size_t security, (const char *)transfer_iterator->bundleHash);
  }
  iota_transaction_t transaction = transaction_new();

  transfer_iterator->current_transfer_transaction_index++;
  return transaction;
}

iota_transaction_t transfer_iterator_next_output_transaction(transfer_iterator_t transfer_iterator, const tryte_t *seed) {
  transfer_t transfer = transfer_iterator->transfers[transfer_iterator->current_transfer];
  size_t count = transfer_transactions_count(transfer);
  if (transfer_iterator->current_transfer_transaction_index >= count) {
    transfer_iterator->current_transfer_transaction_index = 0;
    return NULL;
  }
  if (transfer_iterator->current_transfer_transaction_index == 0) {
  }
  iota_transaction_t transaction = transaction_new();
  transfer_iterator->current_transfer_transaction_index++;
  return transaction;
}

iota_transaction_t transfer_iterator_next_input_transaction(transfer_iterator_t transfer_iterator, const tryte_t *seed) {
  transfer_t transfer = transfer_iterator->transfers[transfer_iterator->current_transfer];
  size_t count = transfer_transactions_count(transfer);
  if (transfer_iterator->current_transfer_transaction_index >= count) {
    transfer_iterator->current_transfer_transaction_index = 0;
    return NULL;
  }  
  if (transfer_iterator->current_transfer_transaction_index == 0) {
  }
  iota_transaction_t transaction = transaction_new();
  transfer_iterator->current_transfer_transaction_index++;
  return transaction;
}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/
// Returns the next transaction
iota_transaction_t transfer_iterator_next(transfer_iterator_t transfer_iterator, const tryte_t *seed) {
  if (transfer_iterator->current_transfer < transfer_iterator->transfers_count) {
    transfer_t transfer = transfer_iterator->transfers[transfer_iterator->current_transfer];
    switch (transfer_type(transfer)) {
      case DATA:
        return transfer_iterator_next_data_transaction(transfer_iterator, seed);
      case VALUE_OUT:
        return transfer_iterator_next_output_transaction(transfer_iterator, seed);
      case VALUE_IN:
        return transfer_iterator_next_input_transaction(transfer_iterator, seed);
    }
  }
  return NULL;
}

// Creates and returns a new transfer iterator
transfer_iterator_t transfer_iterator_new(transfer_t *transfers, size_t len, Kerl* kerl) {
  transfer_iterator_t transfer_iterator;
  transfer_iterator = (transfer_iterator_t)malloc(sizeof(struct _transfer_iterator));
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
  memcpy(transfer_iterator->bundle_hash, transfer_ctx_finalize(transfer_ctx), 81);
  transfer_ctx_free(transfer_ctx);
  return transfer_iterator;
}

// Free an existing transfer iterator - compatible with free()
void transfer_iterator_free(void *t) {
  if (t) {
    free(t);
  }
}
