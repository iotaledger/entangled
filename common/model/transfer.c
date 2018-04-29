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

#define TRYTES_PER_MESSAGE 2187

/***********************************************************************************************************
 * Transfer Input data structure
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _transfer_input {...};

void _convert_and_pad(int64_t value, tryte_t *trytes, size_t length) {
  size_t num_trytes;
  num_trytes = long_to_trytes(value, trytes);
  memset(trytes + num_trytes, '9', length - num_trytes);
}

// Creates and returns a new transfer input
transfer_input_t transfer_input_new(void) {
  transfer_input_t transfer_input;
  transfer_input = (transfer_input_t)malloc(sizeof(struct _transfer_input));
  if (!transfer_input) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_input, 0, sizeof(struct _transfer_input));
  return transfer_input;
}

// Get the seed
tryte_t *transfer_input_seed(transfer_input_t transfer_input) {
  return transfer_input->seed;
}

// Set the seed (copy argument)
void transfer_input_set_seed(transfer_input_t transfer_input, const tryte_t *seed) {
  memcpy(transfer_input->seed, seed, sizeof(transfer_input->seed));
}

// Get the security level
size_t transfer_input_security(transfer_input_t transfer_input) {
  return transfer_input->security;
}

// Set the security level
void transfer_input_set_security(transfer_input_t transfer_input, size_t security) {
  transfer_input->security = security;
}

// Get the index
size_t transfer_input_index(transfer_input_t transfer_input) {
  return transfer_input->index;
}

// Set the index
void transfer_input_set_index(transfer_input_t transfer_input, size_t index) {
  transfer_input->index = index;
}

// Free an existing transfer input - compatible with free()
void transfer_input_free(void *t) {
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

// Get the transfer input
transfer_input_t transfer_value_in_input(transfer_value_in_t transfer_value_in) {
  return transfer_value_in->input;
}

// Set the transfer input (copy argument)
void transfer_value_in_set_input(transfer_value_in_t transfer_value_in, transfer_input_t transfer_input) {
  if (transfer_value_in->input) {
    transfer_value_in_free(transfer_value_in->input);
    transfer_value_in->input = NULL;
  }
  transfer_input_t input = transfer_input_new();
  transfer_input_set_seed(input, transfer_input_seed(transfer_input));
  transfer_input_set_security(input, transfer_input_security(transfer_input));
  transfer_input_set_index(input, transfer_input_index(transfer_input));
  transfer_value_in->input = input;
}

// Get the number of transactions for this value in transfer
size_t transfer_value_in_transactions_count(transfer_value_in_t transfer_value_in) {
  return transfer_input_security(transfer_value_in_input(transfer_value_in));
}

// Creates and returns a new transfer value in
transfer_value_in_t transfer_value_in_new(void) {
  transfer_value_in_t transfer_value_in;
  transfer_value_in = (transfer_value_in_t)malloc(sizeof(struct _transfer_value_in));
  if (!transfer_value_in) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_value_in, 0, sizeof(struct _transfer_value_in));
  transfer_value_in->input = transfer_input_new();
  if (!transfer_value_in->input) {
    // errno = IOTA_OUT_OF_MEMORY
    transfer_value_in_free(transfer_value_in);
    transfer_value_in = NULL;
  }
  return transfer_value_in;
}

// Free an existing transfer value in - compatible with free()
void transfer_value_in_free(void *t) {
  if (t) {
    transfer_value_in_t transfer_value_in = (transfer_value_in_t)t;
    if (transfer_value_in->input) {
      transfer_input_free(transfer_value_in->input);
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

// Get the length of the data
size_t transfer_value_out_len(transfer_value_out_t transfer_value_out) {
  return transfer_value_out->len;
}

// Get the data
tryte_t *transfer_value_out_data(transfer_value_out_t transfer_value_out) {
  return transfer_value_out->data;
}

// Set the data (copy argument)
void transfer_value_out_set_data(transfer_value_out_t transfer_value_out, tryte_t *data, size_t len) {
  if (transfer_value_out->data) {
    free(transfer_value_out->data);
    transfer_value_out->data = NULL;
  }
  size_t data_len = len * sizeof(tryte_t);
  transfer_value_out->data = (tryte_t *)malloc(data_len);
  if (transfer_value_out->data) {
    memcpy(transfer_value_out->data, data, data_len);
  }  
}

// Get the number of transactions for this value out transfer
size_t transfer_value_out_transactions_count(transfer_value_out_t transfer_value_out) {
  return 1;
}

// Creates and returns a new transfer value out
transfer_value_out_t transfer_value_out_new(void) {
  transfer_value_out_t transfer_value_out;
  transfer_value_out = (transfer_value_out_t)malloc(sizeof(struct _transfer_value_out));
  if (!transfer_value_out) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(transfer_value_out, 0, sizeof(struct _transfer_value_out));
  return transfer_value_out;
}

// Free an existing transfer value out - compatible with free()
void transfer_value_out_free(void *t) {
  if (t) {
    transfer_value_out_t transfer_value_out = (transfer_value_out_t)t;
    if (transfer_value_out->data) {
      free(transfer_value_out->data);
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
    case VALUE_IN:
      return transfer_value_in_transactions_count(transfer_value_in(transfer));
    case VALUE_OUT:
      return transfer_value_out_transactions_count(transfer_value_out(transfer));
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
    case VALUE_IN:
      return transfer_value_in_value(transfer_value_in(transfer));
    case VALUE_OUT:
      return transfer_value_out_value(transfer_value_out(transfer));
  }
  // Should not reach here
  return 0;
}

// Get the transfer value
tryte_t *transfer_address(transfer_t transfer) {
  switch (transfer_type(transfer)) {
    case DATA:
      return transfer_data_address(transfer_data(transfer));
    case VALUE_IN:
      return transfer_value_in_address(transfer_value_in(transfer));
    case VALUE_OUT:
      return transfer_value_out_address(transfer_value_out(transfer));
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
transfer_value_in_t transfer_value_in(transfer_t transfer) {
  if (transfer_type(transfer) != VALUE_IN) {
    return NULL;
  }
  return transfer->in;
}

// Get the transfer value out
transfer_value_out_t transfer_value_out(transfer_t transfer) {
  if (transfer_type(transfer) != VALUE_OUT) {
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
    case VALUE_IN:
      transfer->in = transfer_value_in_new();
      break;
    case VALUE_OUT:
      transfer->out = transfer_value_out_new();
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
      case VALUE_IN:
        transfer_value_in_free(transfer_value_in(transfer));
        break;
      case VALUE_OUT:
        transfer_value_out_free(transfer_value_out(transfer));
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
      int64_t effective_value = transfer_type(transfer) == VALUE_IN ? j == 0 ? transfer_value(transfer) : 0 : 0;
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
tryte_t *transfer_ctx_finilize(transfer_ctx_t transfer_ctx) {
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
