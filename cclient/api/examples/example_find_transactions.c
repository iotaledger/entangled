/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

retcode_t find_transactions_by_address(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_ERROR;
  find_transactions_req_t *find_tran = find_transactions_req_new();
  flex_trit_t tmp_hash[FLEX_TRIT_SIZE_243];
  if (!find_tran) {
    printf("Error: OOM, new request\n");
    return RC_OOM;
  }

  ret_code = flex_trits_from_trytes(
      tmp_hash, NUM_TRITS_HASH,
      (const tryte_t *)"UFEANGKZESXDWJCGIYFFUXACPZFQRWMIYVFYOWAZIJPOGFDQTLKAUOILKETJEGYDYMYJAWAXPOQIWCCSY",
      NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  if ((ret_code = hash243_queue_push(&find_tran->addresses, tmp_hash)) != RC_OK) {
    printf("Error: add a hash to queue failed.\n");
    goto err;
  }

  find_transactions_res_t *find_tran_res = find_transactions_res_new();
  if (!find_tran_res) {
    printf("Error: OOM, new response\n");
    goto err;
  }

  ret_code = iota_client_find_transactions(s, find_tran, find_tran_res);

  if (ret_code == RC_OK) {
    size_t count = hash243_queue_count(find_tran_res->hashes);
    hash243_queue_t curr = find_tran_res->hashes;
    for (size_t i = 0; i < count; i++) {
      printf("[%ld] ", i);
      flex_trit_print(curr->hash, NUM_TRITS_HASH);
      printf("\n");
      curr = curr->next;
    }
    printf("tx count = %ld\n", count);
  }

err:
  if (ret_code) {
    printf("find tx failed: %s\n", error_2_string(ret_code));
  }
  find_transactions_req_free(&find_tran);
  find_transactions_res_free(&find_tran_res);
  return ret_code;
}

retcode_t find_transactions_by_tag(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_ERROR;
  find_transactions_req_t *find_tran = find_transactions_req_new();
  flex_trit_t tmp_tag[FLEX_TRIT_SIZE_81];
  if (!find_tran) {
    printf("Error: OOM, new request\n");
    return RC_OOM;
  }

  flex_trits_from_trytes(tmp_tag, NUM_TRITS_TAG, (const tryte_t *)"CCLIENT99999999999999999999", NUM_TRYTES_TAG,
                         NUM_TRYTES_TAG);

  if ((ret_code = hash81_queue_push(&find_tran->tags, tmp_tag)) != RC_OK) {
    printf("Error: add a hash to queue failed.\n");
    goto err;
  }

  find_transactions_res_t *find_tran_res = find_transactions_res_new();
  if (!find_tran_res) {
    printf("Error: OOM, new response\n");
    goto err;
  }

  ret_code = iota_client_find_transactions(s, find_tran, find_tran_res);

  if (ret_code == RC_OK) {
    size_t count = hash243_queue_count(find_tran_res->hashes);
    hash243_queue_t curr = find_tran_res->hashes;
    for (size_t i = 0; i < count; i++) {
      printf("[%ld] ", i);
      flex_trit_print(curr->hash, NUM_TRITS_HASH);
      printf("\n");
      curr = curr->next;
    }
    printf("tx count = %ld\n", count);
  }

err:
  if (ret_code) {
    printf("find tx failed: %s\n", error_2_string(ret_code));
  }
  find_transactions_req_free(&find_tran);
  find_transactions_res_free(&find_tran_res);
  return ret_code;
}

void example_find_transactions(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  find_transactions_by_address(s);
  find_transactions_by_tag(s);
}