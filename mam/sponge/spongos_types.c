/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/sponge/spongos_types.h"
#include "mam/pb3/pb3.h"

size_t mam_spongos_serialized_size(mam_spongos_t const *const spongos) {
  return pb3_sizeof_size_t(spongos->pos) + MAM_SPONGE_WIDTH;
}

void mam_spongos_serialize(mam_spongos_t const *const spongos, trits_t *const trits) {
  pb3_encode_size_t(spongos->pos, trits);
  pb3_encode_ntrytes(trits_from_rep(MAM_SPONGE_WIDTH, spongos->sponge.state), trits);
}

retcode_t mam_spongos_deserialize(trits_t *const trits, mam_spongos_t *const spongos) {
  retcode_t err;
  ERR_BIND_RETURN(pb3_decode_size_t(&spongos->pos, trits), err);
  ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_SPONGE_WIDTH, spongos->sponge.state), trits), err);
  return err;
}
