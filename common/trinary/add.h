#ifndef __TRIT_ADD_H__
#define __TRIT_ADD_H__

#include "common/trinary/trits.h"

trit_t trit_sum(trit_t a, trit_t b);
int add_assign(trit_t *t, size_t s, int64_t v);
void add_trits(const trit_t *const lh, trit_t *rh, size_t len);

#endif  //__TRIT_ADD_H__
