/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CONF_H__
#define __CONSENSUS_CONF_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_consensus_conf_s {
} iota_consensus_conf_t;

retcode_t iota_consensus_conf_init(iota_consensus_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_CONF_H__
