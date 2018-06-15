#ifndef __MAM_MASK_H__
#define __MAM_MASK_H__

#include "common/curl-p/trit.h"

int mask(trit_t *dest, trit_t *message, size_t l, Curl *const c);
int unmask(trit_t *dest, trit_t *cipher, size_t l, Curl *const c);

#endif  //__MAM_MASK_H__
