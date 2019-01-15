/*!
\file troika.h
\brief Troika wrappers.
*/
#pragma once
#include <troika/troika.h"
#include "mam/v2/defs.h"
#include "mam/v2/sponge.h"
#include "mam/v2/spongos.h"

/*#define MAM2_TROIKA_NUM_ROUNDS 12*/
#define MAM2_TROIKA_NUM_ROUNDS 24

typedef struct _troika_sponge {
  isponge s;
  Trit stack[MAM2_SPONGE_WIDTH];
  sponge_state_t state;
} troika_sponge;

typedef struct _troika_spongos {
  ispongos sg;
  troika_sponge s;
} troika_spongos;

isponge *troika_sponge_init(troika_sponge *t);
ispongos *troika_spongos_init(troika_spongos *t);
