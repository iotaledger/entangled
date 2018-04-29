/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_MODEL_TRANSFER_H_
#define __COMMON_MODEL_TRANSFER_H_

#include <stdlib.h>
#include "common/trinary/tryte.h"
#include "common/kerl/kerl.h"

/***********************************************************************************************************
 * Transfer Input data structure
 ***********************************************************************************************************/
typedef struct _transfer_input *transfer_input_t;
struct _transfer_input {
  tryte_t seed[81];
  size_t security;
  size_t index;
};

// Get the seed
tryte_t *transfer_input_seed(transfer_input_t transfer_input);
// Set the seed (copy argument)
void transfer_input_set_seed(transfer_input_t transfer_input, const tryte_t *seed);
// Get the security level
size_t transfer_input_security(transfer_input_t transfer_input);
// Set the security level
void transfer_input_set_security(transfer_input_t transfer_input, size_t security);
// Get the index
size_t transfer_input_index(transfer_input_t transfer_input);
// Set the index
void transfer_input_set_index(transfer_input_t transfer_input, size_t index);
// Creates and returns a new transfer input
transfer_input_t transfer_input_new(void);
// Free an existing transfer input - compatible with free()
void transfer_input_free(void *transfer_input);

/***********************************************************************************************************
 * Transfer Data data structure
 ***********************************************************************************************************/
typedef struct _transfer_data *transfer_data_t;
struct _transfer_data {
  tryte_t address[81];
  size_t len;
  tryte_t* data;
};

// Get the address
tryte_t *transfer_data_address(transfer_data_t transfer_data);
// Set the address (copy argument)
void transfer_data_set_address(transfer_data_t transfer_data, const tryte_t *address);
// Get the length of the data
size_t transfer_data_len(transfer_data_t transfer_data);
// Get the data
tryte_t *transfer_data_data(transfer_data_t transfer_data);
// Set the data (copy argument)
void transfer_data_set_data(transfer_data_t transfer_data, tryte_t *data, size_t len);
// Get the number of transactions for this data transfer
size_t transfer_data_transactions_count(transfer_data_t transfer_data);
// Creates and returns a new transfer data
transfer_data_t transfer_data_new(void);
// Free an existing transfer data - compatible with free()
void transfer_data_free(void *transfer_data);

/***********************************************************************************************************
 * Transfer Value In data structure
 ***********************************************************************************************************/
typedef struct _transfer_value_in *transfer_value_in_t;
struct _transfer_value_in {
  tryte_t address[81];
  int64_t value;
  transfer_input_t input;
};

// Get the address
tryte_t *transfer_value_in_address(transfer_value_in_t transfer_value_in);
// Set the address (copy argument)
void transfer_value_in_set_address(transfer_value_in_t transfer_value_in, const tryte_t *address);
// Get the value
int64_t transfer_value_in_value(transfer_value_in_t transfer_value_in);
// Set the value
void transfer_value_in_set_value(transfer_value_in_t transfer_value_in, int64_t value);
// Get the transfer input
transfer_input_t transfer_value_in_input(transfer_value_in_t transfer_value_in);
// Set the transfer input (copy argument)
void transfer_value_in_set_input(transfer_value_in_t transfer_value_in, transfer_input_t transfer_input);
// Get the number of transactions for this value in transfer
size_t transfer_value_in_transactions_count(transfer_value_in_t transfer_value_in);
// Creates and returns a new transfer value in
transfer_value_in_t transfer_value_in_new(void);
// Free an existing transfer value in - compatible with free()
void transfer_value_in_free(void *transfer_value_in);

/***********************************************************************************************************
 * Transfer Value Out data structure
 ***********************************************************************************************************/
typedef struct _transfer_value_out *transfer_value_out_t;
struct _transfer_value_out {
  tryte_t address[81];
  uint64_t value;
  // can have up to 2187 trytes as data!
  size_t len;
  tryte_t* data;
};

// Get the address
tryte_t *transfer_value_out_address(transfer_value_out_t transfer_value_out);
// Set the address (copy argument)
void transfer_value_out_set_address(transfer_value_out_t transfer_value_out, const tryte_t *address);
// Get the value
int64_t transfer_value_out_value(transfer_value_out_t transfer_value_out);
// Set the value
void transfer_value_out_set_value(transfer_value_out_t transfer_value_out, int64_t value);
// Get the length of the data
size_t transfer_value_out_len(transfer_value_out_t transfer_value_out);
// Get the data
tryte_t *transfer_value_out_data(transfer_value_out_t transfer_value_out);
// Set the data (copy argument)
void transfer_value_out_set_data(transfer_value_out_t transfer_value_out, tryte_t *data, size_t len);
// Get the number of transactions for this value out transfer
size_t transfer_value_out_transactions_count(transfer_value_out_t transfer_value_out);
// Creates and returns a new transfer value out
transfer_value_out_t transfer_value_out_new(void);
// Free an existing transfer value out - compatible with free()
void transfer_value_out_free(void *transfer_value_out);

/***********************************************************************************************************
 * Transfer data structure
 ***********************************************************************************************************/
typedef enum {
  DATA = 0,
  VALUE_IN,
  VALUE_OUT
} transfer_type_e;

typedef struct _transfer *transfer_t;
struct _transfer {
  transfer_type_e type;
  tryte_t tag[27];
  uint64_t timestamp;
  union {
    transfer_data_t data;
    transfer_value_in_t in;
    transfer_value_out_t out;
  };
};

// Get the type
transfer_type_e transfer_type(transfer_t transfer);
// Get the number of transactions for this transfer
size_t transfer_transactions_count(transfer_t transfer);
// Get the tag
tryte_t *transfer_tag(transfer_t transfer);
// Set the tag (copy argument)
void transfer_set_tag(transfer_t transfer, tryte_t *tag);
// Get the timestamp
uint64_t transfer_timestamp(transfer_t transfer);
// Set the timestamp
void transfer_set_timestamp(transfer_t transfer, uint64_t timestamp);
// Get the transfer value
int64_t transfer_value(transfer_t transfer);
// Get the transfer value
tryte_t *transfer_address(transfer_t transfer);
// Get the transfer data or NULL if type not DATA
transfer_data_t transfer_data(transfer_t transfer);
// Get the transfer value in or NULL if type not VALUE_IN
transfer_value_in_t transfer_value_in(transfer_t transfer);
// Get the transfer value out or NULL if type not VALUE_OUT
transfer_value_out_t transfer_value_out(transfer_t transfer);
// Creates and returns a new transfer
transfer_t transfer_new(transfer_type_e transfer_type);
// Free an existing transfer - compatible with free()
void transfer_free(void *transfer);

/***********************************************************************************************************
 * Transfer Context data structure
 ***********************************************************************************************************/
typedef struct _transfer_ctx *transfer_ctx_t;
struct _transfer_ctx {
  size_t count;
  tryte_t bundle[81];
};

// Creates and returns a new transfer context
int transfer_ctx_init(transfer_ctx_t transfer_ctx, transfer_t *transfers, size_t len);
// Calculates the bundle hash for a collection of transfers
int transfer_ctx_hash(transfer_ctx_t transfer_ctx, Kerl* kerl, transfer_t *transfers, size_t len);
// Returns the resulting bundle hash
tryte_t *transfer_ctx_finalize(transfer_ctx_t transfer_ctx);
// Creates and returns a new transfer context
transfer_ctx_t transfer_ctx_new(void);
// Free an existing transfer context - compatible with free()
void transfer_ctx_free(void *transfer_ctx);

#endif  // __COMMON_MODEL_TRANSFER_H_
#ifdef __cplusplus
}
#endif
                               