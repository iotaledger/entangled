#include "pow.h"

#include <stdlib.h>
#include "common/curl-p/hashcash.h"
#include "common/trinary/trit_tryte.h"


#define NONCE_LENGTH 27 * 3


char* do_pow(Curl* curl, const char* trits_in, size_t trits_len, uint8_t mwm) {

    tryte_t* nonce_trits =
            (tryte_t*)calloc(NONCE_LENGTH + 1, sizeof(tryte_t));

    curl_absorb(curl, trits_in, trits_len - HASH_LENGTH);
    memcpy(curl->state, trits_in + trits_len - HASH_LENGTH, HASH_LENGTH);

    // FIXME(th0br0) deal with result value of `hashcash` call
    hashcash(curl, BODY, HASH_LENGTH - NONCE_LENGTH, HASH_LENGTH, mwm);

    memcpy(nonce_trits, curl->state + HASH_LENGTH - NONCE_LENGTH, NONCE_LENGTH);

    return (char*)nonce_trits;
}

