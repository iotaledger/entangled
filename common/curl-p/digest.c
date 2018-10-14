/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/curl-p/digest.h"
#include "common/trinary/trit_tryte.h"

void curl_digest(trit_t const* const trits, size_t const len, trit_t* const out,
                 Curl* const curl) {
  trit_t digest[HASH_LENGTH_TRIT];
  curl_absorb(curl, trits, len);
  curl_squeeze(curl, digest, HASH_LENGTH_TRIT);
  memcpy((tryte_t*)out, digest, HASH_LENGTH_TRIT);
}
