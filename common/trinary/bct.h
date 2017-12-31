#ifndef __COMMON_TRINARY_BCT_H__
#define __COMMON_TRINARY_BCT_H__

#include <stdint.h>
#include <unistd.h>

#include "common/trinary/trits.h"

typedef uint8_t bct_t;

void copy_bct(bct_t *, size_t, bct_t *, size_t, size_t);
void copy_bct_to_trits(trit_t *t, bct_t *s, size_t i, size_t l);
void write_trit(bct_t *, int, trit_t);
trit_t get_trit(bct_t *data, int index);

#endif
