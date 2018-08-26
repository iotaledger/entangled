#include "common/helpers/checksum.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/kerl/hash.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

IOTA_EXPORT char* iota_checksum(const char* input, const size_t inputLength,
                                const size_t checksumLength) {
  Kerl kerl;
  init_kerl(&kerl);

  if (checksumLength == 0) {
    return NULL;
  }

  trit_t* inputTrits = calloc(inputLength * RADIX, sizeof(trit_t));
  trit_t trits_hash[HASH_LENGTH];
  char* checksumTrytes = calloc(checksumLength, sizeof(tryte_t));

  trytes_to_trits((tryte_t*)input, inputTrits, inputLength);
  kerl_hash(inputTrits, inputLength * RADIX, trits_hash, &kerl);
  trits_to_trytes((trit_t*)(&trits_hash[HASH_LENGTH - checksumLength * RADIX]),
                  (tryte_t*)checksumTrytes, checksumLength * RADIX);

  free(inputTrits);

  return checksumTrytes;
}

IOTA_EXPORT flex_trit_t* iota_flex_checksum(const flex_trit_t* flex_trits,
                                            const size_t num_trits,
                                            const size_t checksumLength) {
  Kerl kerl;
  init_kerl(&kerl);

  if (checksumLength == 0) {
    return NULL;
  }

  trit_t trits_hash[HASH_LENGTH];
  trit_t* trits = (trit_t*)calloc(num_trits, sizeof(trit_t));
  flex_trits_to_trits(trits, num_trits, flex_trits, num_trits, num_trits);
  kerl_hash(trits, num_trits, trits_hash, &kerl);
  free(trits);
  size_t flex_len = num_flex_trits_for_trits(num_trits);
  flex_trit_t* checksum_flex_trits =
      (flex_trit_t*)calloc(flex_len, sizeof(flex_trit_t));
  flex_trits_from_trits(checksum_flex_trits, HASH_LENGTH,
                        &trits_hash[HASH_LENGTH - checksumLength],
                        HASH_LENGTH, checksumLength);

  return checksum_flex_trits;
}
