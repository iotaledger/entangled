/*!
\file troika.c
\brief Troika wrappers.
*/
#include "troika.h"
#include <troika/troika.h>
#include "trits.h"

/*void TroikaPermutation(Trit *state, unsigned long num_rounds)*/
static void troika_transform(void *stack, word_t *state) {
  trits_t s;
  Trit *t;
  size_t i;
  trit_t x;
  s = trits_from_rep(MAM2_WORDS(MAM2_SPONGE_WIDTH), state);
  t = (Trit *)stack;

  for (i = 0; i != MAM2_SPONGE_WIDTH; ++i, s = trits_drop(s, 1)) {
    x = trits_get1(s);
    t[i] = x < 0 ? 2 : (Trit)x;
  }

  TroikaPermutation(t, MAM2_TROIKA_NUM_ROUNDS);

  for (i = 0; i != MAM2_SPONGE_WIDTH; ++i, s = trits_drop(s, 1)) {
    x = t[i] > 1 ? -1 : (trit_t)t[i];
    trits_put1(s, x);
  }
}

isponge *troika_sponge_init(troika_sponge *t) {
  t->s.f = troika_transform;
  t->s.stack = t->stack;
  t->s.s = t->state;
  return &t->s;
}

ispongos *troika_spongos_init(troika_spongos *t) {
  t->sg.s = troika_sponge_init(&t->s);
  return &t->sg;
}
