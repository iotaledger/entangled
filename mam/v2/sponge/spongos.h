/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file spongos.h
\brief MAM2 Spongos layer.
*/
#ifndef __MAM_V2_SPONGE_SPONGOS_H__
#define __MAM_V2_SPONGE_SPONGOS_H__

#include "mam/v2/defs.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spongos_s {
  sponge_t *sponge;
  size_t pos;
} spongos_t;

void spongos_fork(spongos_t *s, spongos_t *fork);

void spongos_init(spongos_t *s);

void spongos_commit(spongos_t *s);

void spongos_absorb(spongos_t *s, trits_t X);

void spongos_absorbn(spongos_t *s, size_t n, trits_t *Xs);

void spongos_squeeze(spongos_t *s, trits_t Y);

bool spongos_squeeze_eq(spongos_t *s, trits_t Y);

void spongos_encr(spongos_t *s, trits_t X, trits_t Y);

void spongos_decr(spongos_t *s, trits_t X, trits_t Y);

void spongos_hash(spongos_t *s, trits_t X, trits_t Y);

void spongos_hashn(spongos_t *s, size_t n, trits_t *Xs, trits_t Y);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_SPONGE_SPONGOS_H__
