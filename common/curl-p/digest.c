#include <string.h>

#include "common/trinary/trit_tryte.h"
#include "digest.h"


void curl_digest(trit_t const* const trits, size_t const len, trit_t* const out,
                 Curl* const curl) {
  trit_t digest[HASH_LENGTH];
  curl_absorb(curl, trits, len);
  curl_squeeze(curl, digest, HASH_LENGTH);
  memcpy((tryte_t*)out, digest, HASH_LENGTH);
}
