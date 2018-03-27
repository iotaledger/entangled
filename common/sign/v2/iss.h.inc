#include <string.h>
#include "common/trinary/add.h"

#define ISS_FRAGMENTS 27
#define ISS_KEY_LENGTH HASH_LENGTH *ISS_FRAGMENTS

#define CAT(A, ...) _CAT(A, __VA_ARGS__)
#define _CAT(A, ...) A ## __VA_ARGS__

#define _ISS_PREFIX(A) CAT(CAT(CAT(iss_,HASH_PREFIX),_),A)

int _ISS_PREFIX(subseed)(trit_t *const seed, trit_t *const out, int64_t index,
                HASH_STATE *const c);

int _ISS_PREFIX(key)(trit_t *const s, trit_t *k, const size_t l, HASH_STATE *const c);

int _ISS_PREFIX(key_digest)(trit_t *k, trit_t *o, const size_t l, HASH_STATE *const c);
int _ISS_PREFIX(address)(trit_t *const a, trit_t *const d, size_t l, HASH_STATE *const c);

int _ISS_PREFIX(signature)(trit_t *sig, trit_t *const hash, size_t o, trit_t *key, size_t kl,
                  HASH_STATE *const c);

int _ISS_PREFIX(sig_digest)(trit_t *const d, trit_t *const hash, size_t o, trit_t *sig,
                   size_t sig_len, HASH_STATE *const c);

#undef _ISS_PREFIX
#undef CAT
#undef _CAT
