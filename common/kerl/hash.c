#include "hash.h"
#include "common/trinary/tryte.h"

void kerl_hash(const trit_t* const trits, size_t len, trit_t* out, Kerl* kerl) {
  trit_t digest[HASH_LENGTH];
  kerl_absorb(kerl, trits, len);
  kerl_squeeze(kerl, digest, HASH_LENGTH);
  memcpy((tryte_t*)out, digest, HASH_LENGTH);
}
