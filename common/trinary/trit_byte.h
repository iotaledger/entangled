#ifndef __COMMON_TRINARY_TRIT_BYTE_H__
#define __COMMON_TRINARY_TRIT_BYTE_H__

#include "common/trinary/bytes.h"
#include "common/trinary/trits.h"

size_t min_bytes(size_t);
byte_t trits_to_byte(trit_t *, byte_t, size_t);
void trits_to_bytes(trit_t *, byte_t *, size_t);
void bytes_to_trits(byte_t *, size_t, trit_t *, size_t);

#endif
