/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
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
#ifndef __MAM_PB3_PB3_H__
#define __MAM_PB3_PB3_H__

#include "common/defs.h"
#include "common/errors.h"
#include "mam/defs.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

#define pb3_sizeof_oneof pb3_sizeof_tryte
#define pb3_sizeof_repeated pb3_sizeof_size_t

/**
 * Advances trits buffer by 'n'
 *
 * @param[in, out] buffer - The buffer
 * @param[in] n - The number of trits to advance
 *
 * @return return the advanced buffer
 */
trits_t pb3_trits_take(trits_t *const buffer, size_t const n);

/**
 * return the number of trits in a tryte
 *
 * @return return the number of trits in a tryte
 */
size_t pb3_sizeof_tryte();

/**
 * Encodes a tryte into trits buffer
 *
 * @param[in] tryte - The tryte
 * @param[out] buffer - The buffer
 *
 * @return
 */
void pb3_encode_tryte(tryte_t const tryte, trits_t *const buffer);

/**
 * Decodes a tryte from a trits buffer
 *
 * @param[out] tryte - The tryte
 * @param[in, out] buffer - The buffer
 *
 * @return error code
 */
retcode_t pb3_decode_tryte(tryte_t *const tryte, trits_t *const buffer);

/**
 * return the number of trits in a trint
 *
 * @return return the number of trits in a trint
 */
size_t pb3_sizeof_trint();

/**
 * Encodes a trint into trits buffer
 *
 * @param[in] trint - The trint
 * @param[out]buffer - The buffer
 *
 * @return
 */
void pb3_encode_trint(trint9_t const trint, trits_t *const buffer);

/**
 * Decodes a trint from a trits buffer
 *
 * @param[out] trint - The trint
 * @param[in, out] buffer - The buffer
 *
 * @return error code
 */
retcode_t pb3_decode_trint(trint9_t *const trint, trits_t *const buffer);

/**
 * return the number of trits in a long trint
 *
 * @return return the number of trits in a long trint
 */
size_t pb3_sizeof_longtrint();

/**
 * Encodes a long trint into trits buffer
 *
 * @param[in] trint - The trint
 * @param[out] buffer - The buffer
 *
 * @return
 */
void pb3_encode_longtrint(trint18_t const trint, trits_t *const buffer);

/**
 * Decodes a long trint from a trits buffer
 *
 * @param[out] trint - The trint
 * @param[in, out] buffer - The buffer
 *
 * @return error code
 */
retcode_t pb3_decode_longtrint(trint18_t *const trint, trits_t *const buffer);

/**
 * return the number of trits in a size_t
 *
 * @return return the number of trits in a size_t
 */
size_t pb3_sizeof_size_t(size_t const n);

/**
 * Encodes a size_t into trits buffer
 *
 * @param[in] size - The size
 * @param[out] buffer - The buffer
 *
 * @return
 */
void pb3_encode_size_t(size_t size, trits_t *const buffer);

/**
 * Decodes a size_t from a trits buffer
 *
 * @param[out] size - The size
 * @param[in, out] buffer - The buffer
 *
 * @return error code
 */
retcode_t pb3_decode_size_t(size_t *const size, trits_t *const buffer);

/**
 * return the number of trits in  'n' trytes
 *
 * @param[in] n - The number of trytes
 * @return return the number of trits in 'n' trytes
 */
size_t pb3_sizeof_ntrytes(size_t const n);

/**
 * Encodes 'n' trytes into trits buffer
 *
 * @param[in] ntrytes - The trytes
 * @param[out] buffer - The buffer
 *
 * @return
 */
void pb3_encode_ntrytes(trits_t const ntrytes, trits_t *const buffer);

/**
 * Decodes 'n' trytes from a trits buffer
 *
 * @param[out] ntrytes - The trytes
 * @param[in, out] buffer - The buffer
 *
 * @return error code
 */
retcode_t pb3_decode_ntrytes(trits_t const ntrytes, trits_t *const buffer);

/**
 * Protobuf3 spongos modifier handling
 */

/**
 * Encodes a tryte into buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[out] buffer - The buffer
 * @param[in] tryte - The tryte
 *
 * @return
 */
void pb3_wrap_absorb_tryte(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t const tryte);

/**
 * Decodes a tryte from buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[in, out] buffer - The buffer
 * @param[in] tryte - The tryte
 *
 * @return error code
 */
retcode_t pb3_unwrap_absorb_tryte(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t *const tryte);

/**
 * Encodes a trint into buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[out] buffer - The buffer
 * @param[in] trint - The tryte
 *
 * @return
 */
void pb3_wrap_absorb_trint(mam_spongos_t *const spongos, trits_t *const buffer, trint9_t const trint);

/**
 * Decodes a trint from buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[in, out] buffer - The buffer
 * @param[out] trint - The tryte
 *
 * @return error code
 */
retcode_t pb3_unwrap_absorb_trint(mam_spongos_t *const spongos, trits_t *const buffer, trint9_t *const trint);

/**
 * Encodes a size_t into buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[out] buffer - The buffer
 * @param[in] size - The size
 *
 * @return
 */
void pb3_wrap_absorb_size_t(mam_spongos_t *const spongos, trits_t *const buffer, size_t const size);

/**
 * Decodes a size_t from buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[in, out] buffer - The buffer
 * @param[out] size - The size
 *
 * @return error code
 */
retcode_t pb3_unwrap_absorb_size_t(mam_spongos_t *const spongos, trits_t *const buffer, size_t *const size);

/**
 * Encodes 'n' trytes into buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[out] buffer - The buffer
 * @param[in] trits - The trytes as trits
 *
 * @return
 */
void pb3_wrap_absorb_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);

/**
 * Decodes 'n' trytes from buffer and absorbs its trits into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[in, out] buffer - The buffer
 * @param[out] trits - The trytes as trits
 *
 * @return error code
 */
retcode_t pb3_unwrap_absorb_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);

/**
 * Encrypts 'n' trytes into buffer and alters spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[out] buffer - The buffer
 * @param[in] trits - The trytes as trits
 *
 * @return
 */
void pb3_wrap_crypt_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);

/**
 * Decrypts 'n' trytes from a buffer and alters spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[in, out] buffer - The buffer
 * @param[out] trits - The trytes as trits
 *
 * @return error code
 */
retcode_t pb3_unwrap_crypt_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);

/**
 * Squeezes 'n' trytes into buffer and alters spongos state
 *
 * @param[in, out] spongos - The spongos state
 * @param[out] buffer - The buffer
 * @param[in] n - The number of trytes to squeeze
 *
 * @return
 */
void pb3_wrap_squeeze_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, size_t const n);

/**
 * Squeezes 'n' trytes from spongos state and compares with first 'n' trytes in buffer
 *
 * @param[in, out] spongos - The spongos state
 * @param[out] buffer - The buffer
 * @param[in] n - The number of trytes to squeeze
 *
 * @return error code, error will occur if squeezed state is not equal to buffer's n first trytes
 */
retcode_t pb3_unwrap_squeeze_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, size_t const n);

/**
 * Absorbs 'n' trytes into spongos state
 *
 * @param[out] spongos - The spongos state
 * @param[in] trits - The trits to absorb
 *
 * @return
 */
void pb3_absorb_external_ntrytes(mam_spongos_t *const spongos, trits_t const trits);

/**
 * Squeezes 'n' trytes into buffer
 *
 * @param[in, out] spongos - The spongos state
 * @param[out] trits - The trits to absorb
 *
 * @return
 */
void pb3_squeeze_external_ntrytes(mam_spongos_t *const spongos, trits_t const trits);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_PB3_PB3_H__

/** @} */
