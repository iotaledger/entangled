/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdbool.h>

#include "common/model/transfer.h"
#include "common/sign/normalize.h"

#define TRANSFER_LOGGER_ID "transfer"

static logger_id_t logger_id;

/***********************************************************************************************************
 * Private interface
 ***********************************************************************************************************/

static void transfer_iterator_next_data_transaction(
    transfer_iterator_t* transfer_iterator, transfer_t* transfer) {
  transfer_data_t* trans_data = NULL;
  uint32_t data_len = 0, offset = 0, len = 0;
  size_t flex_ret = 0;

  if (transfer->type == DATA) {
    trans_data = (transfer_data_t*)transfer->meta;
    // Length of message in trits
    data_len = trans_data->len;
    // Offset of message data for current index
    offset = transfer_iterator->current_transfer_transaction_index *
             NUM_TRITS_SIGNATURE;
    // Length of the message data for the current transaction
    len = data_len - offset;
    len = len > NUM_TRITS_SIGNATURE ? NUM_TRITS_SIGNATURE : len;
    flex_ret = flex_trits_slice(
        transfer_iterator->transaction->data.signature_or_message, len,
        trans_data->data, data_len, offset, len);
    if (flex_ret == 0) {
      log_warning(logger_id, "[%s:%d] flex_trits slicing failed.\n", __func__,
                  __LINE__);
    }
  } else {
    log_error(logger_id, "[%s:%d] the transfer type doesn't match.\n", __func__,
              __LINE__);
  }
}

static void transfer_iterator_next_output_transaction(
    transfer_iterator_t* transfer_iterator, transfer_t* transfer) {
  transfer_value_out_t* output_info = NULL;
  iota_transaction_t* tx = NULL;
  size_t flex_ret = 0;

  if (transfer->type == VALUE_OUT) {
    output_info = (transfer_value_out_t*)transfer->meta;
    tx = transfer_iterator->transaction;
    if (transfer_iterator->current_transfer_transaction_index == 0) {
      if (transfer_iterator->transaction_signature) {
        free(transfer_iterator->transaction_signature);
      }
      iota_signature_generator iota_signature_gen =
          transfer_iterator->iota_signature_gen;
      transfer_iterator->transaction_signature =
          (flex_trit_t*)iota_signature_gen(
              output_info->seed, output_info->seed_index, output_info->security,
              transfer_iterator->bundle_hash);
      transaction_set_value(tx, transfer->value);
    }
    flex_ret = flex_trits_slice(
        tx->data.signature_or_message, NUM_TRITS_SIGNATURE,
        transfer_iterator->transaction_signature,
        NUM_TRITS_SIGNATURE * output_info->security,
        NUM_TRITS_SIGNATURE *
            transfer_iterator->current_transfer_transaction_index,
        NUM_TRITS_SIGNATURE);
    if (flex_ret == 0) {
      log_warning(logger_id, "[%s:%d] flex_trits slicing failed.\n", __func__,
                  __LINE__);
    }
  } else {
    log_error(logger_id, "[%s:%d] the transfer type doesn't match.\n", __func__,
              __LINE__);
  }
}

static void transfer_iterator_next_input_transaction(
    transfer_iterator_t* transfer_iterator, transfer_t* transfer) {
  transfer_value_in_t* value_in = NULL;
  iota_transaction_t* tx = transfer_iterator->transaction;

  if (transfer->type == VALUE_IN) {
    value_in = (transfer_value_in_t*)transfer->meta;
    memcpy(transaction_signature(tx), value_in->data, value_in->len);
    transaction_set_value(tx, transfer->value);
  } else {
    log_error(logger_id, "[%s:%d] the transfer type doesn't match.\n", __func__,
              __LINE__);
  }
}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/

void transfer_logger_init() {
  logger_id = logger_helper_enable(TRANSFER_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(logger_id, "Enable logger %s.\n", TRANSFER_LOGGER_ID);
}

void transfer_logger_destroy() {
  log_info(logger_id, "Destroy logger %s.\n", TRANSFER_LOGGER_ID);
  logger_helper_release(logger_id);
}

bool validate_output(transfer_value_out_t const* const output) {
  if (!output->seed) {
    log_error(logger_id, "[%s:%d] seed is NULL.\n", __func__, __LINE__);
    return false;
  }

  if (output->security < 1 || output->security > 3) {
    log_error(logger_id, "[%s:%d] security leve error.\n", __func__, __LINE__);
    return false;
  }
  return true;
}

// create data transfer object.
transfer_t* transfer_data_new(flex_trit_t const* const address,
                              flex_trit_t const* const tag,
                              flex_trit_t const* const data, uint32_t data_len,
                              uint64_t timestamp) {
  transfer_data_t* tf_data = NULL;
  transfer_t* transfer = NULL;
  if (!address) {
    log_error(logger_id, "[%s:%d] address cannot be NULL.\n", __func__,
              __LINE__);
    return NULL;
  }

  if (!tag) {
    log_error(logger_id, "[%s:%d] tag cannot be NULL.\n", __func__, __LINE__);
    return NULL;
  }

  if (!data && data_len != 0) {
    log_error(logger_id, "[%s:%d] data length should be 0.\n", __func__,
              __LINE__);
    return NULL;
  }

  tf_data = (transfer_data_t*)calloc(1, sizeof(transfer_data_t));
  if (tf_data) {
    transfer = (transfer_t*)calloc(1, sizeof(transfer_t));
    if (!transfer) {
      free(tf_data);
      log_error(logger_id, "[%s:%d] Out of Memory.\n", __func__, __LINE__);
      return NULL;
    }

    tf_data->data = data;
    tf_data->len = data_len;

    transfer->type = DATA;
    transfer->address = address;
    transfer->value = 0;
    transfer->tag = tag;
    transfer->timestamp = timestamp;
    transfer->meta = (void*)tf_data;
  }
  return transfer;
}

// create a value_out transfer object.
transfer_t* transfer_value_out_new(transfer_value_out_t const* const output,
                                   flex_trit_t const* const tag,
                                   flex_trit_t const* const address,
                                   int64_t value, uint64_t timestamp) {
  transfer_value_out_t* value_out = NULL;
  transfer_t* tf = NULL;
  if (!validate_output(output)) {
    log_error(logger_id, "[%s:%d] output is invalid.\n", __func__, __LINE__);
    return NULL;
  }

  if (!address) {
    log_error(logger_id, "[%s:%d] address cannot be NULL.\n", __func__,
              __LINE__);
    return NULL;
  }

  if (!tag) {
    log_error(logger_id, "[%s:%d] tag cannot be NULL.\n", __func__, __LINE__);
    return NULL;
  }

  tf = (transfer_t*)calloc(1, sizeof(transfer_t));
  if (!tf) {
    log_error(logger_id, "[%s:%d] Out of Memory.\n", __func__, __LINE__);
    return NULL;
  }

  value_out = (transfer_value_out_t*)calloc(1, sizeof(transfer_value_out_t));
  if (!value_out) {
    free(tf);
    return NULL;
  }
  memcpy(value_out, output, sizeof(transfer_value_out_t));

  tf->type = VALUE_OUT;
  tf->address = address;
  tf->value = value;
  tf->tag = tag;
  tf->timestamp = timestamp;
  tf->meta = (void*)value_out;
  return tf;
}

// create a value_int transfer object.
transfer_t* transfer_value_in_new(flex_trit_t const* const address,
                                  flex_trit_t const* const tag, int64_t value,
                                  flex_trit_t const* const data,
                                  uint32_t data_len, uint64_t timestamp) {
  transfer_value_in_t* value_in = NULL;
  transfer_t* tf = NULL;
  if (!address) {
    log_error(logger_id, "[%s:%d] address cannot be NULL.\n", __func__,
              __LINE__);
    return NULL;
  }

  if (!tag) {
    log_error(logger_id, "[%s:%d] tag cannot be NULL.\n", __func__, __LINE__);
    return NULL;
  }

  if (!data && data_len != 0) {
    log_error(logger_id, "[%s:%d] data length should be 0.\n", __func__,
              __LINE__);
    return NULL;
  }

  tf = (transfer_t*)calloc(1, sizeof(transfer_t));
  if (!tf) {
    log_error(logger_id, "[%s:%d] Out of Memory.\n", __func__, __LINE__);
    return NULL;
  }

  value_in = (transfer_value_in_t*)calloc(1, sizeof(transfer_value_in_t));
  if (!value_in) {
    free(tf);
    log_error(logger_id, "[%s:%d] Out of Memory.\n", __func__, __LINE__);
    return NULL;
  }
  value_in->len = data_len;
  value_in->data = data;

  tf->type = VALUE_IN;
  tf->address = address;
  tf->tag = tag;
  tf->value = value;
  tf->timestamp = timestamp;
  tf->meta = (void*)value_in;
  return tf;
}

void transfer_free(transfer_t** transfer) {
  if (transfer && *transfer) {
    free((*transfer)->meta);
    free(*transfer);
    *transfer = NULL;
  }
}

uint32_t transfer_transactions_count(transfer_t* tf) {
  transfer_data_t* data = NULL;
  transfer_value_out_t* value_out = NULL;
  switch (tf->type) {
    case DATA:
      data = (transfer_data_t*)tf->meta;
      return (data->len + NUM_TRITS_SIGNATURE - 1) / NUM_TRITS_SIGNATURE;
    case VALUE_OUT:
      // security level
      value_out = (transfer_value_out_t*)tf->meta;
      return value_out->security;
    case VALUE_IN:
      // fixed
      return 1;
  }
  return 0;
}

transfer_ctx_t* transfer_ctx_new() {
  transfer_ctx_t* tf_ctx = (transfer_ctx_t*)calloc(1, sizeof(transfer_ctx_t));
  if (!tf_ctx) {
    log_error(logger_id, "[%s:%d] Out of Memory.\n", __func__, __LINE__);
    return NULL;
  }
  memset(tf_ctx->bundle, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  return tf_ctx;
}

void transfer_ctx_free(transfer_ctx_t* transfer_ctx) { free(transfer_ctx); }

bool transfer_ctx_init(transfer_ctx_t* transfer_ctx, transfer_t* transfers[],
                       uint32_t len) {
  uint32_t i;
  int64_t total = 0;
  transfer_t* transfer = NULL;
  for (i = 0; i < len; i++) {
    transfer = transfers[i];
    transfer_ctx->count += transfer_transactions_count(transfer);
    total += transfer->value;
  }
  return total ? false : true;
}

// Calculates the bundle hash for a collection of transfers
void transfer_ctx_hash(transfer_ctx_t* transfer_ctx, Kerl* kerl,
                       transfer_t* transfers[], uint32_t tx_len) {
  size_t i, j, count, current_index = 0;
  trit_t essence_trits[NUM_TRITS_ESSENCE];
  trit_t bundle_trit[HASH_LENGTH_TRIT];
  byte_t normalized_hash[HASH_LENGTH_TRYTE];
  transfer_t* tf = NULL;
  int64_t value;
  bool valid_bundle = false;
  trit_t first_tx_tag_trits[NUM_TRITS_TAG];
  flex_trits_to_trits(first_tx_tag_trits, NUM_TRITS_TAG, transfers[0]->tag,
                      NUM_TRITS_TAG, NUM_TRITS_TAG);

  while (!valid_bundle) {
  loop:
    init_kerl(kerl);
    // Calculate bundle hash
    current_index = 0;
    for (i = 0; i < tx_len; i++) {
      tf = transfers[i];
      count = transfer_transactions_count(tf);
      for (j = 0; j < count; j++) {
        value = tf->type == VALUE_OUT ? (j == 0 ? tf->value : 0) : tf->value;
        absorb_essence(kerl, tf->address, value, tf->tag, tf->timestamp,
                       current_index, transfer_ctx->count - 1, essence_trits);
        current_index++;
      }
    }

    // Squeeze kerl to get the bundle hash
    kerl_squeeze(kerl, bundle_trit, HASH_LENGTH_TRIT);
    flex_trits_from_trits(transfer_ctx->bundle, HASH_LENGTH_TRIT, bundle_trit,
                          HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

    // normalize
    normalize_flex_hash(transfer_ctx->bundle, normalized_hash);
    // checking 'M'
    for (i = 0; i < HASH_LENGTH_TRYTE; i++) {
      if (normalized_hash[i] == 13) {
        // Insecure bundle. Increment Tag and recompute bundle hash.
        add_assign(first_tx_tag_trits, NUM_TRITS_TAG, 1);
        flex_trits_from_trits(transfers[0]->tag, NUM_TRITS_TAG,
                              first_tx_tag_trits, NUM_TRITS_TAG, NUM_TRITS_TAG);
        goto loop;
      }
    }
    valid_bundle = true;
  }
}

transfer_iterator_t* transfer_iterator_new(transfer_t* transfers[],
                                           uint32_t len, Kerl* kerl,
                                           iota_transaction_t* transaction) {
  transfer_ctx_t transfer_ctx = {};
  transfer_iterator_t* transfer_iterator =
      (transfer_iterator_t*)calloc(1, sizeof(transfer_iterator_t));
  if (!transfer_iterator) {
    log_error(logger_id, "[%s:%d] Out of Memory.\n", __func__, __LINE__);
    return NULL;
  }

  if (transaction) {
    transfer_iterator->transaction = transaction;
    transfer_iterator->dynamic_transaction = 0;
  } else {
    // Dynamically allocate a transaction structure
    transfer_iterator->transaction = transaction_new();
    transfer_iterator->dynamic_transaction = 1;
  }

  transfer_iterator->transfers = transfers;
  transfer_iterator->transfers_count = len;
  if (transfer_ctx_init(&transfer_ctx, transfers, len)) {
    transfer_iterator->transactions_count = transfer_ctx.count;
    transfer_ctx_hash(&transfer_ctx, kerl, transfers, len);
    memcpy(transfer_iterator->bundle_hash, transfer_ctx.bundle,
           FLEX_TRIT_SIZE_243);
  } else {
    log_error(logger_id, "[%s:%d] Invalid transfers.\n", __func__, __LINE__);
    return NULL;
  }
  transfer_iterator->iota_signature_gen = iota_sign_signature_gen_flex_trits;
  return transfer_iterator;
}

void transfer_iterator_free(transfer_iterator_t** iter) {
  transfer_iterator_t* iter_p = *iter;
  if (iter && *iter) {
    if (iter_p->dynamic_transaction && iter_p->transaction) {
      free(iter_p->transaction);
    }
    if (iter_p->transaction_signature) {
      free(iter_p->transaction_signature);
    }
    free(iter_p);
    *iter = NULL;
  }
}

iota_transaction_t* transfer_iterator_next(
    transfer_iterator_t* transfer_iterator) {
  iota_transaction_t* transaction = NULL;

  if (transfer_iterator->current_transfer <
      transfer_iterator->transfers_count) {
    transfer_t* transfer =
        transfer_iterator->transfers[transfer_iterator->current_transfer];
    uint32_t count = transfer_transactions_count(transfer);
    if (transfer_iterator->current_transfer_transaction_index >= count) {
      transfer_iterator->current_transfer++;
      transfer_iterator->current_transfer_transaction_index = 0;
      return transfer_iterator_next(transfer_iterator);
    }
    transaction = transfer_iterator->transaction;
    // Reset all transaction fields
    transaction_reset(transaction);
    // Set common transaction fields
    transaction_set_bundle(transaction, transfer_iterator->bundle_hash);
    transaction_set_address(transaction, transfer->address);
    transaction_set_tag(transaction, transfer->tag);
    transaction_set_obsolete_tag(transaction, transfer->tag);
    transaction_set_timestamp(transaction, transfer->timestamp);
    transaction_set_current_index(transaction,
                                  transfer_iterator->current_transaction_index);
    transaction_set_last_index(transaction,
                               transfer_iterator->transactions_count - 1);
    transaction->loaded_columns_mask.essence = MASK_ESSENCE_ALL;
    transaction->loaded_columns_mask.data = MASK_DATA_SIG_OR_MSG;

    // Set transaction type specific fields
    switch (transfer->type) {
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
    transfer_iterator->current_transfer_transaction_index++;
    transfer_iterator->current_transaction_index++;
  }
  return transaction;
}

void absorb_essence(Kerl* const kerl, flex_trit_t const* address, int64_t value,
                    flex_trit_t const* obsolete_tag, uint64_t timestamp,
                    int64_t current_index, int64_t last_index,
                    trit_t* const essence_trits) {
  memset(essence_trits, 0, NUM_TRITS_ESSENCE);

  trit_t* curr_pos = essence_trits;

  flex_trits_to_trits(curr_pos, NUM_TRITS_ADDRESS, address, NUM_TRITS_ADDRESS,
                      NUM_TRITS_ADDRESS);
  curr_pos = &curr_pos[NUM_TRITS_ADDRESS];
  long_to_trits(value, curr_pos);
  curr_pos = &curr_pos[NUM_TRITS_VALUE];
  flex_trits_to_trits(curr_pos, NUM_TRITS_OBSOLETE_TAG, obsolete_tag,
                      NUM_TRITS_OBSOLETE_TAG, NUM_TRITS_OBSOLETE_TAG);
  curr_pos = &curr_pos[NUM_TRITS_OBSOLETE_TAG];
  long_to_trits(timestamp, curr_pos);
  curr_pos = &curr_pos[NUM_TRITS_TIMESTAMP];
  long_to_trits(current_index, curr_pos);
  curr_pos = &curr_pos[NUM_TRITS_CURRENT_INDEX];
  long_to_trits(last_index, curr_pos);
  // Absorb essence in kerl
  kerl_absorb(kerl, essence_trits, NUM_TRITS_ESSENCE);
}
