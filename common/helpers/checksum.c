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