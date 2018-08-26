/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_FILES_H__
#define __UTILS_FILES_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t copy_file(const char *to, const char *from);
retcode_t remove_file(const char *file_path);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_FILES_H__
