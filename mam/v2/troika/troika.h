/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_TROIKA_H__
#define __MAM_V2_TROIKA_H__

#include "common/troika/troika.h"
#include "mam/v2/defs.h"

/*#define MAM2_TROIKA_NUM_ROUNDS 12*/
#define MAM2_TROIKA_NUM_ROUNDS 24

void mam_troika_transform(trit_t *const state, size_t state_size);

#endif  //__MAM_V2_TROIKA_H__
