/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_SYSTEM_H
#define __UTILS_SYSTEM_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @return The number of available processor cores
 **/
size_t system_cpu_available();

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_SYSTEM_H
