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
#ifndef __MAM_MAM_ENDPOINT_H__
#define __MAM_MAM_ENDPOINT_H__

#include "mam/mss/mss.h"
#include "mam/trits/trits.h"

#define MAM_ENDPOINT_ID_TRIT_SIZE MAM_MSS_MT_HASH_SIZE
#define MAM_ENDPOINT_ID_TRYTE_SIZE (MAM_ENDPOINT_ID_TRIT_SIZE / 3)
#define MAM_ENDPOINT_NAME_SIZE 18

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_endpoint_s {
  trits_t name_size;
  trits_t name;
  mam_mss_t mss;
} mam_endpoint_t;

typedef struct mam_endpoint_t_set_entry_s mam_endpoint_t_set_entry_t;
typedef mam_endpoint_t_set_entry_t *mam_endpoint_t_set_t;

/**
 * Gets an endpoint's id
 *
 * @param endpoint The endpoint
 *
 * @return the endpoint's id
 */
static inline trits_t mam_endpoint_id(mam_endpoint_t const *const endpoint) {
  return trits_from_rep(MAM_ENDPOINT_ID_TRIT_SIZE, endpoint->mss.root);
}

/**
 * Gets an endpoint's name size
 *
 * @param endpoint The endpoint
 *
 * @return the endpoint's name size
 */
static inline trits_t mam_endpoint_name_size(mam_endpoint_t const *const endpoint) { return endpoint->name_size; }

/**
 * Gets an endpoint's name
 *
 * @param endpoint The endpoint
 *
 * @return the endpoint's name
 */
static inline trits_t mam_endpoint_name(mam_endpoint_t const *const endpoint) { return endpoint->name; }

/**
 * Allocates memory for internal objects and generates MSS public key
 *
 * @param allocator A MAM allocator
 * @param prng A shared PRNG interface used to generate WOTS private keys
 * @param height MSS MT height
 * @param channel_name_size The channel name size
 * @param channel_name The channel name
 * @param endpoint_name The endpoint name
 * @param endpoint The endpoint
 *
 * @return a status code
 */
retcode_t mam_endpoint_create(mam_prng_t *const prng, mss_mt_height_t const height, trits_t const channel_name_size,
                              trits_t const channel_name, trits_t const endpoint_name, mam_endpoint_t *const endpoint);

/**
 * Returns the number of remaining secret keys (unused leaves on merkle tree)
 * @param endpoint The endpoint
 *
 * @return number of remaining secret keys
 */
static inline size_t mam_endpoint_remaining_sks(mam_endpoint_t const *const endpoint) {
  return endpoint ? mam_mss_remaining_sks(&endpoint->mss) : 0;
}

/**
 * Deallocates memory for internal objects
 *
 * @param allocator A MAM allocator
 * @param endpoint The endpoint
 */
void mam_endpoint_destroy(mam_endpoint_t *const endpoint);

retcode_t mam_endpoints_destroy(mam_endpoint_t_set_t *const endpoints);

size_t mam_endpoint_serialized_size(mam_endpoint_t const *const endpoint);

void mam_endpoint_serialize(mam_endpoint_t const *const endpoint, trits_t *const buffer);

retcode_t mam_endpoint_deserialize(trits_t *const buffer, trits_t const channel_name_size, trits_t const channel_name,
                                   mam_prng_t *const prng, mam_endpoint_t *const endpoint);

size_t mam_endpoints_serialized_size(mam_endpoint_t_set_t const endpoints);

void mam_endpoints_serialize(mam_endpoint_t_set_t const endpoints, trits_t *const buffer);

retcode_t mam_endpoints_deserialize(trits_t *const buffer, trits_t const channel_name_size, trits_t const channel_name,
                                    mam_prng_t *const prng, mam_endpoint_t_set_t *const endpoints);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_MAM_ENDPOINT_H__

/** @} */
