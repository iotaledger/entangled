#ifndef __TRIT_ADD_H__
#define __TRIT_ADD_H__

#include "common/trinary/trits.h"

trit_t trit_sum(trit_t const a, trit_t const b);
int add_assign(trit_t *const t, size_t const s, int64_t const v);
void add_trits(trit_t const *const lh, trit_t *const rh, size_t const len);

#endif  //__TRIT_ADD_H__
