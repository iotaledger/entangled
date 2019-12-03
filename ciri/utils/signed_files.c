/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/utils/signed_files.h"
#include "common/crypto/curl-p/trit.h"
#include "common/crypto/iss/normalize.h"
#include "common/crypto/iss/v1/iss_curl.h"
#include "common/crypto/kerl/kerl.h"
#include "common/model/bundle.h"
#include "common/trinary/trit_long.h"
#include "common/trinary/tryte_ascii.h"
#include "utils/merkle.h"

/*
 * Private functions
 */

static retcode_t validate_signature(char const *const signature_filename, flex_trit_t const *const public_key,
                                    uint32_t depth, size_t index, flex_trit_t *const digest, bool *const valid) {
  retcode_t ret = RC_OK;
  Curl curl;
  FILE *fp = NULL;
  ssize_t read = 0;
  char *line = NULL;
  size_t len = 0;
  trit_t normalized_digest[HASH_LENGTH_TRIT];
  trit_t sig_digests[3 * HASH_LENGTH_TRIT];
  trit_t sig_trits[NORMALIZED_FRAGMENT_LENGTH * HASH_LENGTH_TRYTE * RADIX];
  trit_t root[HASH_LENGTH_TRIT];
  trit_t public_key_trits[HASH_LENGTH_TRIT];
  trit_t siblings[depth * HASH_LENGTH_TRYTE * RADIX];

  if ((fp = fopen(signature_filename, "r")) == NULL) {
    return RC_UTILS_FAILED_TO_OPEN_FILE;
  }

  curl.type = CURL_P_81;
  curl_init(&curl);

  normalize_flex_hash_to_trits(digest, normalized_digest);

  int i;
  for (i = 0; i < 3 && (read = getline(&line, &len, fp)) > 0; i++) {
    line[--read] = '\0';
    if (read != NORMALIZED_FRAGMENT_LENGTH * HASH_LENGTH_TRYTE) {
      ret = RC_UTILS_INVALID_SIG_FILE;
      goto done;
    }
    trytes_to_trits((tryte_t *)line, sig_trits, read);
    iss_curl_sig_digest(sig_digests + i * HASH_LENGTH_TRIT, normalized_digest + i * NORMALIZED_FRAGMENT_LENGTH * RADIX,
                        sig_trits, RADIX * read, &curl);
  }
  if (i != 3 || read < 0) {
    ret = RC_UTILS_INVALID_SIG_FILE;
    goto done;
  }

  iss_curl_address(sig_digests, root, sizeof(sig_digests), &curl);
  if ((read = getline(&line, &len, fp)) > 0) {
    line[--read] = '\0';
    if ((size_t)read != depth * HASH_LENGTH_TRYTE) {
      ret = RC_UTILS_INVALID_SIG_FILE;
      goto done;
    }
    trytes_to_trits((tryte_t *)line, siblings, read);
    merkle_root(root, siblings, depth, index, &curl);
  }
  flex_trits_to_trits(public_key_trits, HASH_LENGTH_TRIT, public_key, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  *valid = (memcmp(public_key_trits, root, HASH_LENGTH_TRIT) == 0);

done:
  if (fp) {
    fclose(fp);
  }
  if (line) {
    free(line);
  }
  return ret;
}

static retcode_t digest_file(char const *const filename, flex_trit_t *const digest) {
  retcode_t ret = RC_OK;
  FILE *fp = NULL;
  ssize_t read = 0;
  char *line = NULL;
  size_t len = 0;
  tryte_t *trytes = NULL;
  trit_t *trits = NULL;
  trit_t digest_trits[HASH_LENGTH_TRIT];
  Kerl kerl;

  if ((fp = fopen(filename, "r")) == NULL) {
    return RC_UTILS_FAILED_TO_OPEN_FILE;
  }

  kerl_init(&kerl);
  while ((read = getline(&line, &len, fp)) > 0) {
    line[--read] = '\0';
    // 2 trytes by ASCII character
    if ((trytes = (tryte_t *)realloc(trytes, read * 2)) == NULL) {
      ret = RC_OOM;
      goto done;
    }
    ascii_to_trytes(line, trytes);
    // 3 trits by tryte and size needs to be a multiple of HASH_LENGTH_TRIT
    // (kerl)
    if ((trits = (trit_t *)realloc(trits, HASH_LENGTH_TRIT * (((read * 6) / HASH_LENGTH_TRIT) + 1))) == NULL) {
      ret = RC_OOM;
      goto done;
    }
    memset(trits, 0, HASH_LENGTH_TRIT * (((read * 6) / HASH_LENGTH_TRIT) + 1));
    trytes_to_trits(trytes, trits, read * 2);
    kerl_absorb(&kerl, trits, HASH_LENGTH_TRIT * (((read * 6) / HASH_LENGTH_TRIT) + 1));
  }
  kerl_squeeze(&kerl, digest_trits, HASH_LENGTH_TRIT);
  flex_trits_from_trits(digest, HASH_LENGTH_TRIT, digest_trits, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

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
  if (trits) {
    free(trits);
  }

  return ret;
}

/*
 * Public functions
 */

retcode_t iota_file_signature_validate(char const *const filename, char const *const signature_filename,
                                       flex_trit_t const *const public_key, uint32_t depth, size_t index,
                                       bool *const valid) {
  retcode_t ret = RC_OK;
  flex_trit_t digest[FLEX_TRIT_SIZE_243];

  if ((ret = digest_file(filename, digest))) {
    return ret;
  }
  return validate_signature(signature_filename, public_key, depth, index, digest, valid);
}
