/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef ENTANGLED_COMMON_FILES_H
#define ENTANGLED_COMMON_FILES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int copy_file(const char *to, const char *from);

#ifdef __cplusplus
}
#endif

#endif  // ENTANGLED_COMMON_FILES_H