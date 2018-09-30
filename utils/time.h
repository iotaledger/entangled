/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_TIME_H__
#define __UTILS_TIME_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t current_timestamp_ms();
void sleep_ms(uint64_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_TIME_H__
