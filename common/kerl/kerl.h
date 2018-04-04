#ifndef __COMMON_KERL_KERL_H_
#define __COMMON_KERL_KERL_H_

#include <keccak/KeccakHash.h>

#include "common/curl-p/const.h"
#include "common/stdint.h"
#include "common/trinary/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  Keccak_HashInstance keccak;
} Kerl;

void init_kerl(Kerl* ctx);

void kerl_absorb(Kerl* ctx, trit_t* trits, size_t length);
void kerl_squeeze(Kerl* ctx, trit_t* trits, size_t length);
void kerl_reset(Kerl* ctx);

#ifdef __cplusplus
}
#endif

#endif
