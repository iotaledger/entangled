/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/transaction.h"
#include "common/trinary/tryte_long.h"
#include <string.h>
#include <stdlib.h>

/***********************************************************************************************************
 * Transaction
 ***********************************************************************************************************/
// To obsucate the model, the structure definition comees here
// struct _iota_transaction {...}

/***********************************************************************************************************
 * Private interface
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in bytes
// Return non 0 on success
uint16_t transaction_deserialize_from_trytes(iota_transaction transaction, const tryte_t *trytes) {
    uint16_t offset = 0;
    transaction_set_signature(transaction, trytes + offset);
    offset+= sizeof(transaction->signature_or_message);
    transaction_set_address(transaction, trytes + offset);
    offset+= sizeof(transaction->address);
    transaction_set_value(transaction, trytes_to_long(trytes + offset, 27));
    offset+= 27;
    transaction_set_obsolete_tag(transaction, trytes_to_long(trytes + offset, 27));
    offset+= 27;
    transaction_set_timestamp(transaction, trytes_to_long(trytes + offset, 9));
    offset+= 9;
    transaction_set_current_index(transaction, trytes_to_long(trytes + offset, 9));
    offset+= 9;
    transaction_set_last_index(transaction, trytes_to_long(trytes + offset, 9));
    offset+= 9;
    transaction_set_bundle(transaction, trytes + offset);
    offset+= sizeof(transaction->bundle);
    transaction_set_trunk(transaction, trytes + offset);
    offset+= sizeof(transaction->trunk);
    transaction_set_branch(transaction, trytes + offset);
    offset+= sizeof(transaction->branch);
    transaction_set_tag(transaction, trytes + offset);
    offset+= sizeof(transaction->tag);
    transaction_set_attachment_timestamp(transaction, trytes_to_long(trytes + offset, 9));
    offset+= 9;
    transaction_set_attachment_timestamp_lower(transaction, trytes_to_long(trytes + offset, 9));
    offset+= 9;
    transaction_set_attachment_timestamp_upper(transaction, trytes_to_long(trytes + offset, 9));
    offset+= 9;
    transaction_set_nonce(transaction, trytes + offset);
    offset+= sizeof(transaction->nonce);
    return offset;
}

void _convert_and_pad(int64_t value, tryte_t *trytes, size_t length) {
    size_t num_trytes;
    num_trytes = long_to_trytes(value, trytes);
    memset(trytes + num_trytes, '9', length - num_trytes);
}
// Serialize an existing transaction
// Return non 0 on success
uint16_t transaction_serialize_to_trytes(const iota_transaction transaction, tryte_t *trytes) {
    uint16_t offset = 0;
    memcpy(trytes + offset, transaction_signature(transaction), sizeof(transaction->signature_or_message));
    offset+= sizeof(transaction->signature_or_message);
    memcpy(trytes + offset, transaction_address(transaction), sizeof(transaction->address));
    offset+= sizeof(transaction->address);
    _convert_and_pad(transaction_value(transaction), trytes + offset, 27);
    offset+= 27;
    _convert_and_pad(transaction_obsolete_tag(transaction), trytes + offset, 27);
    offset+= 27;
    _convert_and_pad(transaction_timestamp(transaction), trytes + offset, 9);
    offset+= 9;
    _convert_and_pad(transaction_current_index(transaction), trytes + offset, 9);
    offset+= 9;
    _convert_and_pad(transaction_last_index(transaction), trytes + offset, 9);
    offset+= 9;
    memcpy(trytes + offset, transaction_bundle(transaction), sizeof(transaction->bundle));
    offset+= sizeof(transaction->bundle);
    memcpy(trytes + offset, transaction_trunk(transaction), sizeof(transaction->trunk));
    offset+= sizeof(transaction->trunk);
    memcpy(trytes + offset, transaction_branch(transaction), sizeof(transaction->branch));
    offset+= sizeof(transaction->branch);
    memcpy(trytes + offset, transaction_tag(transaction), sizeof(transaction->tag));
    offset+= sizeof(transaction->tag);
    _convert_and_pad(transaction_attachment_timestamp(transaction), trytes + offset, 9);
    offset+= 9;
    _convert_and_pad(transaction_attachment_timestamp_lower(transaction), trytes + offset, 9);
    offset+= 9;
    _convert_and_pad(transaction_attachment_timestamp_upper(transaction), trytes + offset, 9);
    offset+= 9;
    memcpy(trytes + offset, transaction_nonce(transaction), sizeof(transaction->nonce));
    offset+= sizeof(transaction->nonce);
    return offset;
}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/
/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
// Get the transaction signature
tryte_t *transaction_signature(iota_transaction transaction) {
    return transaction->signature_or_message;
}

// Set the transaction signature (copy argument)
void transaction_set_signature(iota_transaction transaction, const tryte_t *signature) {
    memcpy(transaction->signature_or_message, signature, sizeof(transaction->signature_or_message));
}

// Get the transaction message
tryte_t *transaction_message(iota_transaction transaction) {
    return transaction->signature_or_message;
}

// Set the transaction message (copy argument)
void transaction_set_message(iota_transaction transaction, const tryte_t *message) {
    memcpy(transaction->signature_or_message, message, sizeof(transaction->signature_or_message));
}

// Get the transaction address
tryte_t *transaction_address(iota_transaction transaction) {
    return transaction->address;
}

// Set the transaction address (copy argument)
void transaction_set_address(iota_transaction transaction, const tryte_t *address) {
    memcpy(transaction->address, address, sizeof(transaction->address));
}

// Get the transaction value
int64_t transaction_value(iota_transaction transaction) {
    return transaction->value;
}

// Set the transaction value
void transaction_set_value(iota_transaction transaction, int64_t value) {
    transaction->value = value;
}

// Get the transaction obsolete tag
uint64_t transaction_obsolete_tag(iota_transaction transaction) {
    return transaction->obsolete_tag;
}

// Set the transaction obsolete tag
void transaction_set_obsolete_tag(iota_transaction transaction, uint64_t obsolete_tag) {
    transaction->obsolete_tag = obsolete_tag;
}

// Get the transaction timestamp
uint64_t transaction_timestamp(iota_transaction transaction) {
    return transaction->timestamp;
}

// Set the transaction timestamp
void transaction_set_timestamp(iota_transaction transaction, uint64_t timestamp) {
    transaction->timestamp = timestamp;
}

// Get the transaction current index
int64_t transaction_current_index(iota_transaction transaction) {
    return transaction->current_index;
}

// Set the transaction current index
void transaction_set_current_index(iota_transaction transaction, int64_t index) {
    transaction->current_index = index;
}

// Get the transaction last index
int64_t transaction_last_index(iota_transaction transaction) {
    return transaction->last_index;
}

// Set the transaction last index
void transaction_set_last_index(iota_transaction transaction, int64_t index) {
    transaction->last_index = index;
}

// Get the transaction bundle
tryte_t *transaction_bundle(iota_transaction transaction) {
    return transaction->bundle;
}

// Set the transaction bundle (copy argument)
void transaction_set_bundle(iota_transaction transaction, const tryte_t *bundle) {
    memcpy(transaction->bundle, bundle, sizeof(transaction->bundle));
}

// Get the transaction trunk
tryte_t *transaction_trunk(iota_transaction transaction) {
    return transaction->trunk;
}

// Set the transaction trunk (copy argument)
void transaction_set_trunk(iota_transaction transaction, const tryte_t *trunk) {
    memcpy(transaction->trunk, trunk, sizeof(transaction->trunk));
}

// Get the transaction branch
tryte_t *transaction_branch(iota_transaction transaction) {
    return transaction->branch;
}

// Set the transaction branch (copy argument)
void transaction_set_branch(iota_transaction transaction, const tryte_t *branch) {
    memcpy(transaction->branch, branch, sizeof(transaction->branch));
}

// Get the transaction tag
tryte_t *transaction_tag(iota_transaction transaction) {
    return transaction->tag;
}

// Set the transaction tag (copy argument)
void transaction_set_tag(iota_transaction transaction, const tryte_t *tag) {
    memcpy(transaction->tag, tag, sizeof(transaction->tag));
}

// Get the transaction attachement timestamp
int64_t transaction_attachment_timestamp(iota_transaction transaction) {
    return transaction->attachment_timestamp;
}

// Set the transaction attachement timestamp
void transaction_set_attachment_timestamp(iota_transaction transaction, int64_t timestamp) {
    transaction->attachment_timestamp = timestamp;
}

// Get the transaction attachement timestamp lower
int64_t transaction_attachment_timestamp_lower(iota_transaction transaction) {
    return transaction->attachment_timestamp_lower;
}

// Set the transaction attachement timestamp lower
void transaction_set_attachment_timestamp_lower(iota_transaction transaction, int64_t timestamp) {
    transaction->attachment_timestamp_lower = timestamp;
}

// Get the transaction attachement timestamp upper
int64_t transaction_attachment_timestamp_upper(iota_transaction transaction) {
    return transaction->attachment_timestamp_upper;    
}

// Set the transaction attachement timestamp upper
void transaction_set_attachment_timestamp_upper(iota_transaction transaction, int64_t timestamp) {
    transaction->attachment_timestamp_upper = timestamp;
}

// Get the transaction nonce
tryte_t *transaction_nonce(iota_transaction transaction) {
    return transaction->nonce;
}

// Set the transaction nonce (copy argument)
void transaction_set_nonce(iota_transaction transaction, const tryte_t *nonce) {
    memcpy(transaction->nonce, nonce, sizeof(transaction->nonce));
}

/***********************************************************************************************************
 * Constructors
 ***********************************************************************************************************/
// Creates and returns a new transaction without data
iota_transaction transaction_new(void) {
    iota_transaction transaction;
    transaction = (iota_transaction)malloc(sizeof(struct _iota_transaction));
    if (!transaction) {
        // errno = IOTA_OUT_OF_MEMORY
    }
    return transaction;
}

// Creates and returns a new transaction from serialized data
// Returns NULL if failed
iota_transaction transaction_deserialize(const tryte_t *trytes) {
    iota_transaction transaction;
    transaction = transaction_new();
    if (!transaction) {
        // errno = IOTA_OUT_OF_MEMORY
        return NULL;
    }
    if (!transaction_deserialize_from_trytes(transaction, trytes)) {
        // errno = IOTA_SOME_ERROR
        transaction_free(transaction);
        return NULL;
    }
    return transaction;
}

/***********************************************************************************************************
 * Serialization
 ***********************************************************************************************************/
// Returns the serialized data from an existing transaction
// Returns NULL if failed
tryte_t *transaction_serialize(const iota_transaction transaction) {
    tryte_t *serialized_value = (tryte_t *)malloc(sizeof(tryte_t) * 2673);
    if (!serialized_value) {
        // errno = IOTA_OUT_OF_MEMORY
        return NULL;
    }
    if (!transaction_serialize_on_trytes(transaction, serialized_value)) {
        free(serialized_value);
        serialized_value = NULL;
    }
    return serialized_value;
}

// Places the serialized data from an existing transaction in pre-allocated bytes
// Returns 0 on success
uint16_t transaction_serialize_on_trytes(const iota_transaction transaction, tryte_t *trytes) {
    return transaction_serialize_to_trytes(transaction, trytes);
}
/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
// Free an existing transaction - compatible with free()
void transaction_free(void *t) {
    free(t);
}

