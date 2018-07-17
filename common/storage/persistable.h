/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_PERSISTABLE_H__
#define __COMMON_STORAGE_PERSISTABLE_H__

#include <stdint.h>
#include "common/trinary/trit_array.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct persistable_base persistable_t;

typedef struct {
    //get the table name
    const char* (*table_name)(const persistable_t* const);
    // get the key of the persistable for lookup
    const trit_array_p (*key)(const persistable_t* const);
    // get the value of the persistable for storing
    const trit_array_p (*value)(const persistable_t* const);
    // get the object from the trits
    const void* (*obj_from_trits)(trit_array_p);
} persistable_vtable;

typedef struct persistable_base {
    persistable_vtable vtable;
    void* derived_obj;
} persistable_base;

typedef struct {
    persistable_t base;
    //transaction_t data;
} transaction_view_model;

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_PERSISTABLE_H__
