/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup mam
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef __MAM_TROIKA_TROIKA_H__
#define __MAM_TROIKA_TROIKA_H__

#include "common/crypto/ftroika/ftroika.h"
#include "common/crypto/troika/troika.h"
#include "mam/defs.h"

#define MAM_TROIKA_NUM_ROUNDS 24

void mam_ftroika_transform(trit_t *const state, size_t state_size);
void mam_troika_transform(trit_t *const state, size_t state_size);

#endif  //__MAM_TROIKA_TROIKA_H__

/** @} */
