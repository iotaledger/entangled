/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_STORAGE_H__
#define __COMMON_STORAGE_STORAGE_H__

#include <stdint.h>
#include "common/trinary/trit_array.h"
#include "common/errors.h"
#include "common/storage/persistable.h"

#ifdef __cplusplus
extern "C" {
#endif
    
extern retcode_t iota_stor_store(const persistable_t* const);
extern retcode_t iota_stor_load(persistable_t* const);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_STORAGE_H__
