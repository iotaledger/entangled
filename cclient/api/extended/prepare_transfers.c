/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <math.h>
#include <stdbool.h>

#include "cclient/api/core/get_balances.h"
#include "cclient/api/extended/get_inputs.h"
#include "cclient/api/extended/get_new_address.h"
#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/prepare_transfers.h"
#include "utils/containers/hash/hash243_queue.h"
#include "utils/time.h"

static void add_transaction_entry(bundle_transactions_t* bundle, int num_txs, flex_trit_t const* const address,
                                  int64_t value, flex_trit_t* tag, uint64_t timestamp) {
  iota_transaction_t tx = {};
  transaction_reset(&tx);
  for (int i = 0; i < num_txs; i++) {
    transaction_set_address(&tx, address);
    transaction_set_tag(&tx, tag);
    transaction_set_timestamp(&tx, timestamp);
    if (i == 0) {  // put value in first transaction only.
      transaction_set_value(&tx, value);
    } else {
      transaction_set_value(&tx, 0);
    }
    transaction_set_obsolete_tag(&tx, tag);
    bundle_transactions_add(bundle, &tx);
  }
}

// add remainder and sign inputs
static retcode_t add_remainder(iota_client_service_t const* const serv, flex_trit_t const* const seed, uint8_t security,
                               inputs_t const* const inputs, bundle_transactions_t* const bundle, flex_trit_t* tag,
                               int64_t total, flex_trit_t const* const remainder_address, Kerl* const kerl) {
  retcode_t ret = RC_ERROR;
  int64_t total_transfer_value = total;
  input_t* input = NULL;

  INPUTS_FOREACH(inputs->input_array, input) {
    int64_t input_value = input->balance;
    int64_t substract = 0 - input_value;
    // timestamp is in secondes
    uint64_t timestamp = current_timestamp_ms() / 1000;
    if (input_value == 0) {
      continue;
    }

    // add input to bundle
    add_transaction_entry(bundle, security, input->address, substract, tag, timestamp);

    if (input_value >= total_transfer_value) {
      int64_t remainder_value = input_value - total_transfer_value;
      if (remainder_value > 0 && remainder_address != NULL) {
        // put remainder value to remainder address
        add_transaction_entry(bundle, 1, remainder_address, remainder_value, tag, timestamp);
        // sign and return
        return bundle_sign(bundle, seed, inputs, kerl);
      } else if (remainder_value > 0) {
        // remainder address is needed
        address_opt_t addr_info = {0, 0, security};
        hash243_queue_t new_addresses = NULL;
        if ((ret = iota_client_get_new_address(serv, seed, addr_info, &new_addresses)) == RC_OK) {
          // unused address
          add_transaction_entry(bundle, 1, new_addresses->prev->hash, remainder_value, tag, timestamp);
          hash243_queue_free(&new_addresses);
          return bundle_sign(bundle, seed, inputs, kerl);
        }
        // get address failed
        log_error(client_extended_logger_id, "Getting addresses failed\n");
        hash243_queue_free(&new_addresses);
        return ret;
      } else {
        // no remainder value, simply sign and return
        return bundle_sign(bundle, seed, inputs, kerl);
      }

    } else {
      total_transfer_value -= input_value;
    }
  }
  // total balance is not enough.
  return RC_CCLIENT_INSUFFICIENT_BALANCE;
}

static retcode_t check_balances(iota_client_service_t const* const serv, inputs_t const* const inputs, bool* passed) {
  retcode_t ret = RC_ERROR;
  input_t* input_elm = NULL;

  get_balances_req_t* balances_req = get_balances_req_new();
  get_balances_res_t* balances_res = get_balances_res_new();
  if (balances_req == NULL || balances_res == NULL) {
    ret = RC_OOM;
    log_error(client_extended_logger_id, "Out of Memory\n");
    goto end;
  }

  balances_req->threshold = 100;
  INPUTS_FOREACH(inputs->input_array, input_elm) {
    if ((ret = get_balances_req_address_add(balances_req, input_elm->address)) != RC_OK) {
      log_error(client_extended_logger_id, "Adding addresses to request is failed.\n");
      goto end;
    }
  }

  if ((ret = iota_client_get_balances(serv, balances_req, balances_res)) == RC_OK) {
    size_t index = 0;
    INPUTS_FOREACH(inputs->input_array, input_elm) {
      if (input_elm->balance != (int64_t)get_balances_res_balances_at(balances_res, index)) {
        *passed = false;
        log_error(client_extended_logger_id, "The given balance does not match with the Tangle\n");
        goto end;
      }
      index++;
    }
    *passed = true;
  }

end:
  get_balances_req_free(&balances_req);
  get_balances_res_free(&balances_res);
  return ret;
}

retcode_t iota_client_prepare_transfers(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                        uint8_t security, transfer_array_t const* const transfers,
                                        flex_trit_t const* const remainder, inputs_t const* const inputs,
                                        bool validate_inputs, uint64_t timestamp, bundle_transactions_t* out_bundle) {
  retcode_t ret = RC_ERROR;
  int64_t total_value = 0;
  signature_fragments_t* sign_fragments = signature_fragments_new();
  Kerl kerl;
  kerl_init(&kerl);

  if (timestamp == 0) {
    timestamp = current_timestamp_ms() / 1000;
  }

  // fill in the transfer and inputs
  transfer_t* elm = NULL;
  flex_trit_t* tag = NULL;
  iota_transaction_t tx;
  TRANSFER_FOREACH(transfers, elm) {
    // TODO validate transfer: check if the address is valid(with a trailing zero trit).
    size_t msg_chunks = 1;
    // count message length
    tryte_t msg_buff[NUM_TRYTES_MESSAGE + 1];
    msg_chunks += floor((double)elm->msg_len / NUM_TRYTES_MESSAGE);

    for (size_t i = 0; i < (msg_chunks * NUM_TRYTES_MESSAGE); i += NUM_TRYTES_MESSAGE) {
      transaction_reset(&tx);

      if (i + NUM_TRYTES_MESSAGE > elm->msg_len) {
        int last_chunk = elm->msg_len - ((msg_chunks - 1) * NUM_TRYTES_MESSAGE);
        strncpy((char*)msg_buff, (char*)elm->message + i, last_chunk);
        msg_buff[last_chunk] = '\0';
      } else {
        strncpy((char*)msg_buff, (char*)elm->message + i, NUM_TRYTES_MESSAGE);
        msg_buff[NUM_TRYTES_MESSAGE] = '\0';
      }

      tag = elm->tag;
      signature_fragments_add(sign_fragments, msg_buff);
      transaction_set_address(&tx, elm->address);
      transaction_set_tag(&tx, elm->tag);
      transaction_set_timestamp(&tx, timestamp);

      if (i == 0) {
        transaction_set_value(&tx, elm->value);
      } else {
        transaction_set_value(&tx, 0);
      }

      transaction_set_obsolete_tag(&tx, elm->tag);
      bundle_transactions_add(out_bundle, &tx);
    }
    total_value += elm->value;
  }

  bundle_set_messages(out_bundle, sign_fragments);
  signature_fragments_free(sign_fragments);

  if (total_value != 0) {
    address_opt_t address_info = {0, 0, security};
    if (inputs != NULL && inputs_len(inputs) != 0) {
      // inputs is provided by user.
      // add remainder, depended on inputs
      if (validate_inputs == true) {
        bool valid_balances = false;
        ret = check_balances(serv, inputs, &valid_balances);
        if (ret != RC_OK || valid_balances == false) {
          // balance is not match.
          ret = RC_CCLIENT_CHECK_BALANCE;
          goto end;
        }
      }
      ret = add_remainder(serv, seed, security, inputs, out_bundle, tag, total_value, remainder, &kerl);
    } else {  // get inputs from seed
      inputs_t local_inputs = {};
      ret = iota_client_get_inputs(serv, seed, address_info, total_value, &local_inputs);
      if (ret == RC_OK) {
        // add remainder
        ret = add_remainder(serv, seed, security, &local_inputs, out_bundle, tag, total_value, remainder, &kerl);
      }
      inputs_clear(&local_inputs);
    }
  } else {
    bundle_reset_indexes(out_bundle);
    bundle_finalize(out_bundle, &kerl);
    ret = RC_OK;
  }

#ifdef DEBUG
  bundle_dump(out_bundle);
#endif

end:
  return ret;
}