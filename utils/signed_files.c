/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/curl-p/trit.h"
#include "common/kerl/kerl.h"
#include "common/model/bundle.h"
#include "common/sign/v1/iss_curl.h"
#include "common/trinary/tryte_ascii.h"
#include "utils/merkle.h"
#include "utils/signed_files.h"

retcode_t is_file_signature_valid(char const *const filename,
                                  char const *const signature_filename,
                                  tryte_t const *const public_key, size_t depth,
                                  size_t index, bool *const valid) {
  retcode_t ret = RC_OK;
  trit_t digest[HASH_LENGTH];

  if ((ret = digest_file(filename, digest))) {
    return ret;
  }
  return validate_signature(signature_filename, public_key, depth, index,
                            digest, valid);
}

retcode_t validate_signature(char const *const signature_filename,
                             tryte_t const *const public_key, size_t depth,
                             size_t index, trit_t *digest, bool *const valid) {
  retcode_t ret = RC_OK;
  Curl curl;
  FILE *fp = NULL;
  ssize_t read = 0;
  char *line = NULL;
  size_t len = 0;
  byte_t normalized_digest[81];  // TODO SIZE
  trit_t sig_digests[3 * HASH_LENGTH];
  trit_t root[HASH_LENGTH];
  trit_t public_key_trits[HASH_LENGTH];

  if ((fp = fopen(signature_filename, "r")) == NULL) {
    return RC_UTILS_FAILED_TO_OPEN_FILE;
  }

  curl.type = CURL_P_81;
  init_curl(&curl);

  normalized_bundle(digest, normalized_digest);

  for (int i = 0; i < 3 && (read = getline(&line, &len, fp)) > 0; i++) {
    line[--read] = '\0';
    trit_t sig_trits[3 * read];
    trytes_to_trits((tryte_t *)line, sig_trits, read);
    iss_curl_sig_digest(sig_digests + i * HASH_LENGTH,
                        normalized_digest + i * NORMALIZED_FRAGMENT_LENGTH,
                        sig_trits, 3 * read, &curl);
    curl_reset(&curl);
  }
  iss_curl_address(sig_digests, root, 3 * HASH_LENGTH, &curl);
  if ((read = getline(&line, &len, fp)) > 0) {
    line[--read] = '\0';
    trit_t siblings[3 * read];
    trytes_to_trits((tryte_t *)line, siblings, read);
    merkle_root(root, siblings, depth, index, &curl);
  }
  trytes_to_trits(public_key, public_key_trits, 81);  // TODO SIZE
  *valid = (memcmp(public_key_trits, root, HASH_LENGTH) == 0);
  return ret;
}

retcode_t digest_file(char const *const filename, trit_t *const digest) {
  retcode_t ret = RC_OK;
  FILE *fp = NULL;
  ssize_t read = 0;
  char *line = NULL;
  size_t len = 0;
  tryte_t *trytes = NULL;
  trit_t trits[HASH_LENGTH * 3] = {0};  // TODO change size
  Kerl kerl;

  if ((fp = fopen(filename, "r")) == NULL) {
    return RC_UTILS_FAILED_TO_OPEN_FILE;
  }

  init_kerl(&kerl);
  while ((read = getline(&line, &len, fp)) > 0) {
    line[--read] = '\0';
    if ((trytes = realloc(trytes, read * 2)) == NULL) {
      ret = RC_UTILS_OOM;
      goto done;
    }
    ascii_to_trytes(line, trytes);
    trytes_to_trits(trytes, trits, read * 2);
    kerl_absorb(&kerl, trits, HASH_LENGTH * 3);  // TODO change size
    memset(trits, 0, read * 6);                  // TODO change size
  }
  kerl_squeeze(&kerl, digest, HASH_LENGTH);

done:
  if (fp) {
    fclose(fp);
  }
  if (line) {
    free(line);
  }
  if (trytes) {
    free(trytes);
  }

  return ret;
}
