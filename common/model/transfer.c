/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/transfer.h"
#include "common/trinary/tryte_ascii.h"

UT_icd ut_transfer_icd = {sizeof(transfer_t), NULL, NULL, NULL};

retcode_t transfer_message_set(transfer_t* tf, tryte_t const* const msg) {
  size_t len = strlen((char*)msg) + 1;
  tf->message = (tryte_t*)malloc(sizeof(tryte_t) * len);
  if (tf->message == NULL) {
    return RC_OOM;
  }
  memcpy(tf->message, msg, len);
  tf->message[len] = '\0';
  return RC_OK;
}

transfer_array_t* transfer_array_new() {
  transfer_array_t* transfers = NULL;
  utarray_new(transfers, &ut_transfer_icd);
  return transfers;
}
