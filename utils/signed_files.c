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
#include "common/trinary/tryte_ascii.h"
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
