/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_SYSTEM_H
#define __UTILS_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the number of available processor cores
 **/
size_t system_cpu_available();

#ifdef __cplusplus
}
#endif

#endif // __UTILS_SYSTEM_H 