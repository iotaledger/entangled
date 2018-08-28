/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_MODEL_MILESTONE_H__
#define __COMMON_MODEL_MILESTONE_H__

// Forward declarations
typedef struct _trit_array *trit_array_p;

typedef struct milestone_s {
  size_t index;
  trit_array_p hash;
} milestone_t;

#endif  //  __COMMON_MODEL_MILESTONE_H__
