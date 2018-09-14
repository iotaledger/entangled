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

}
