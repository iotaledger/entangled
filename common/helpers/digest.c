#include "digest.h"
#include "common/curl-p/digest.h"
#include "common/trinary/trit_tryte.h"

#include <string.h>
#include <stdlib.h>

#define TRYTE_LENGTH 2673
#define TRANSACTION_LENGTH TRYTE_LENGTH * 3

char* iota_digest(const char* trytes) {
    Curl curl;
    init_curl(&curl);
    curl.type = CURL_P_81;

    size_t length = strnlen(trytes, TRANSACTION_LENGTH);
    char input[sizeof(char) * length * 3];
    trytes_to_trits((tryte_t*)trytes, input, length);

    char* trits_hash = calloc(HASH_LENGTH + 1, sizeof(char));
    curl_digest(input, length * 3, trits_hash, &curl);

    char* hash = calloc(HASH_LENGTH/3 + 1, sizeof(char));
    trits_to_trytes((trit_t*)trits_hash, (tryte_t*)hash, HASH_LENGTH);
    free(trits_hash);

    return hash;

}
