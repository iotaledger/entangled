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
#pragma once

#include "mam/v2/defs.h"
#include "mam/v2/trits.h"
/*#include "mam/v2/sponge.h"*/

struct _isponge;
typedef struct _ispongos {
  struct _isponge *s;
  size_t pos;
} ispongos;

void spongos_fork(ispongos *s, ispongos *fork);

void spongos_init(ispongos *s);

void spongos_commit(ispongos *s);

void spongos_absorb(ispongos *s, trits_t X);

void spongos_absorbn(ispongos *s, size_t n, trits_t *Xs);

void spongos_squeeze(ispongos *s, trits_t Y);

bool_t spongos_squeeze_eq(ispongos *s, trits_t Y);

void spongos_encr(ispongos *s, trits_t X, trits_t Y);

void spongos_decr(ispongos *s, trits_t X, trits_t Y);

void spongos_hash(ispongos *s, trits_t X, trits_t Y);

void spongos_hashn(ispongos *s, size_t n, trits_t *Xs, trits_t Y);

bool_t spongos_test(ispongos *s);
