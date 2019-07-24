/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/transfer.h"
#include "common/trinary/tryte_ascii.h"

UT_icd ut_transfer_icd = {sizeof(transfer_t), NULL, NULL, NULL};

retcode_t transfer_message_set_trytes(transfer_t* tf, tryte_t const* const trytes, size_t length) {
  tf->message = (tryte_t*)malloc(sizeof(tryte_t) * length);
  if (tf->message == NULL) {
    return RC_OOM;
  }
  memcpy(tf->message, trytes, length);
  tf->msg_len = length;
  return RC_OK;
}

retcode_t transfer_message_set_string(transfer_t* tf, char const* const str) {
  if (strlen(str) == 0) {
    return RC_NULL_PARAM;
  }

  size_t trytes_len = strlen(str) * 2;
  tryte_t trytes_msg[trytes_len];
  ascii_to_trytes(str, trytes_msg);
  return transfer_message_set_trytes(tf, trytes_msg, trytes_len);
}

transfer_array_t* transfer_array_new() {
  transfer_array_t* transfers = NULL;
  utarray_new(transfers, &ut_transfer_icd);
  return transfers;
}
