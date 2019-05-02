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
#ifndef __MAM_MSS_MSS_H__
#define __MAM_MSS_MSS_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/prng/prng.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"
#include "mam/wots/wots.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief MSS public key size. */
#define MAM_MSS_PK_SIZE 243
/*! \brief Trits needed to encode tree depth part of SKN. */
#define MAM_MSS_SKN_TREE_DEPTH_SIZE 4
/*! \brief Trits needed to encode key number part of SKN. */
#define MAM_MSS_SKN_KEY_NUMBER_SIZE 14
/*! \brief Trits needed to encode `skn`: tree depth and key number. */
#define MAM_MSS_SKN_SIZE (MAM_MSS_SKN_TREE_DEPTH_SIZE + MAM_MSS_SKN_KEY_NUMBER_SIZE)
/*! \brief MSS authentication path size of height `d`. */
#define MAM_MSS_APATH_SIZE(d) (MAM_WOTS_PK_SIZE * d)
/*! \brief MSS signature size with a tree of height `d`. */
#define MAM_MSS_SIG_SIZE(d) (MAM_MSS_SKN_SIZE + MAM_WOTS_SIG_SIZE + MAM_MSS_APATH_SIZE(d))
/*! \brief MSS signed hash value size. */
#define MAM_MSS_HASH_SIZE MAM_WOTS_HASH_SIZE

/*! \brief Max Merkle tree height. */
#define MAM_MSS_MAX_D 20

/*! \brief Size of hash values stored in Merkle tree */
#define MAM_MSS_MT_HASH_SIZE MAM_WOTS_PK_SIZE

/*! \brief Leaves have height `0`, root has height `D`; `0 <= d < D`; `D <=
 * 20`.
 */
typedef trint6_t mss_mt_height_t;
/*! \brief Index (skn) of leaf/node on the level of height `d`; 0 <= i <
 * 2^(D-d). */
typedef trint18_t mss_mt_idx_t;

#if defined(MAM_MSS_TRAVERSAL)

/*! \brief Node info, specifies position of the node in the MT.
\note Corresponding hash-value is stored externally. */
typedef struct mss_mt_node_s {
  mss_mt_height_t height; /*!< Height. */
  mss_mt_idx_t level_idx; /*!< Level index. */
} mss_mt_node_t;
/*! \brief Number of auxiliary MT nodes used by tree-traversal algorithm */
#define MAM_MSS_MT_NODES(d) ((d) * ((d) + 1) / 2)

/*! \brief Stack info.
\note Stack nodes are stored externally. */
typedef struct mss_mt_stack_s {
  mss_mt_height_t height; /*!< Height. */
  mss_mt_idx_t level_idx; /*!< Level index. */
  size_t size;            /*!< Size of stack. */
} mss_mt_stack_t;
/*! \brief Number of auxiliary stacks used by tree-traversal algorithm */
#define MAM_MSS_MT_STACKS(d) (d)
#define MAM_MSS_MT_STACK_CAPACITY(d) ((d) + 1)

#define MAM_MSS_HASH_IDX(i) (MAM_MSS_MT_HASH_SIZE * (i))
/*! \brief Memory for hash-values. TODO: Add 1 extra hash for mss_gen. */
#define MAM_MSS_MT_HASH_WORDS(d, i) MAM_MSS_HASH_IDX(MAM_MSS_MT_NODES(d) + (i))
#define MAM_MSS_MT_AUTH_WORDS(d) MAM_MSS_HASH_IDX(d)
#else
/*! \brief MSS Merkle-tree implementation storage words. */
#define MAM_MSS_MT_WORDS(height) (MAM_WOTS_PK_SIZE * (2 * (1 << (height)) - 1))
#endif

#if defined(MAM_MSS_TRAVERSAL)
#define MAM_MSS_MT_MAX_STORED_SIZE(d) ((d) * ((d) + 3) / 2 * MAM_MSS_MT_HASH_SIZE)
#else
#define MAM_MSS_MT_MAX_STORED_SIZE(height) (((1 << ((height) + 1)) - 1) * MAM_MSS_MT_HASH_SIZE)
#endif
#define MAM_MSS_MAX_STORED_SIZE(d) (4 + 14 + MAM_MSS_MT_MAX_STORED_SIZE(d))

/*! \brief MSS interface used to generate public key and sign. */
typedef struct mss_s {
  mss_mt_height_t height; /*!< Merkle tree height. */
  mss_mt_idx_t skn;       /*!< Current WOTS private key number. */
  mam_prng_t *prng;       /*!< PRNG interface used to generate WOTS private keys. */
#if defined(MAM_MSS_TRAVERSAL)
  trit_t *auth_path;      /*!< Current authentication path; `d` hash values. */
  trit_t *nodes_hashes;   /*!< Buffer storing hash-values of auxiliary nodes;
                MAM_MSS_MT_NODES(d) hash-values in total. */
  mss_mt_node_t *nodes;   /*<! Auxiliary node infos. */
  mss_mt_stack_t *stacks; /*<! Stacks used by traversal algorithm. */
#else
  trit_t *mt; /*!< Buffer storing complete Merkle-tree. */
#endif
  trits_t nonce1, nonce2; /*!< Nonce = `N1`||`N2`, stored pointers only, NOT copies. */

  trit_t root[MAM_MSS_PK_SIZE];
} mam_mss_t;

#if defined(MAM_MSS_TRAVERSAL)
/*! \note
MSS key gen requires stack of capacity `D+1`.
In order to save space stack at level `D-1` is reused.
Although stack at level `d` has capacity `d+1`,
for the purpose of key gen stack `D-1` must have capacity `D+1`.
It is achieved by allocating one extra node:

```
  trit_t *alloc_words(size_t word_count);
  mss_mt_node_t *alloc_nodes(size_t node_count);
  mss_mt_stack_t *alloc_stacks(size_t stack_count);
  mss_mt_height_t D;

  size_t total_nodes = MAM_MSS_MT_NODES(D)+1;
  m->height = D;
  m->ap = alloc_words(D * MAM_MSS_MT_HASH_SIZE);
  m->hs = alloc_words(total_nodes * MAM_MSS_MT_HASH_SIZE);
  m->nodes = alloc_nodes(total_nodes);
  m->stacks = alloc_stacks(D);
```
*/
#endif

/**
 * MSS interface initialization
 *
 * @param mss [in] MSS interface
 * @param prng [in] PRNG interface
 * @param sponge [in] Sponge interface
 * @param height [in] Merkle-tree height
 * @param nonce1 [in] first nonce
 * @param nonce2 [in] second nonce
 *
 * @return void
 */

void mam_mss_init(mam_mss_t *mss, mam_prng_t *const prng, mss_mt_height_t height, trits_t nonce1, trits_t nonce2);
/**
 * Generate MSS keys, stores current and next auth_path
 *
 * @param mss [in] MSS interface
 * @return void
 */
void mam_mss_gen(mam_mss_t *mss);

/**
 * Encodes mss height and current sk index
 *
 * @param mss [in] MSS interface
 * @param skn [out] encoded height and current private key number
 * @return void
 */
void mam_mss_skn(mam_mss_t const *const mss, /*!< [in] MSS interface */
                 trits_t skn                 /*!< [out] encoded height and current private key number */
);

/**
 * Gets the authentication path
 *
 * @param mss [in] MSS interface
 * @param skn [in] number of WOTS instance (current pk index), in traversal mode
 * this parameter is not used because current authentication path is always
 * updated
 * @param path [out] authentication path
 *
 * @return void
 */
void mam_mss_auth_path(mam_mss_t *mss, mss_mt_idx_t skn, trits_t path);

/**
 * Signs a hash
 *
 * @param mss [in] MSS interface
 * @param hash [in] the hash to sign on
 * @param sig [out] the signature
 *
 * @return retcode
 */
retcode_t mam_mss_sign(mam_mss_t *mss, trits_t hash, trits_t sig);

/**
 * Signs a hash and advances skn
 *
 * @param mss [in] MSS interface
 * @param hash [in] the hash to sign on
 * @param sig [out] the signature
 *
 * @return retcode
 */
retcode_t mam_mss_sign_and_next(mam_mss_t *mss, trits_t hash, trits_t sig);

/**
 * Advances skn
 *
 * @param mss [in] MSS interface
 *
 * @return True if can produce next signature
 */
bool mam_mss_next(mam_mss_t *mss);

/**
 * Returns the number of remaining secret keys (unused leaves on merkle tree)
 *
 * @param mss [in] MSS interface
 *
 * @return The number of remaining signatures
 */
size_t mam_mss_num_remaining_sks(mam_mss_t const *const mss);

/**
 * Verifies MSS signature.
 *
 * @param mt_spongos [in] Spongos interface to hash Merkle Tree
 * @param hash [in] signed hash value
 * @param sig [in] signature
 * @param [in] public key (Merkle-tree root)
 *
 * @return bool True is the signature is correct, False otherwise
 */
bool mam_mss_verify(mam_spongos_t *mt_spongos, trits_t hash, trits_t sig, trits_t pk);

/**
 * Allocate memory for internal Merkle tree structure.
 *
 * \note `mss_init` must still be called afterwards.
 * \note In case of error `mss_destroy` must be called.
 * \note Non Merkle tree related objects (WOTS, PRNG, Spongos interfaces)
 *     must be allocated separately.
 *
 * @param mss [out] MSS interface
 * @param height [in] the tree's height
 *
 * @return void
 */
retcode_t mam_mss_create(mam_mss_t *mss, mss_mt_height_t height);

/**
 * Deallocate memory for internal Merkle tree structure.
 * \note Pointer `m` must be freed afterwards.
 *
 * @param mss [out] MSS interface
 *
 * @return void
 */
void mam_mss_destroy(mam_mss_t *mss);

/**
 * returns The size of a serialized Merkle tree.
 *
 * @param mss [in] MSS interface
 *
 * @return size_t The size for stored MT
 */

size_t mam_mss_serialized_size(mam_mss_t const *const mss);

/**
 * Serialize Merkle tree.
 *
 * @param mss [in] MSS interface
 * @param buffer [out] The serialized MT buffer
 *
 * @return void
 */

void mam_mss_serialize(mam_mss_t const *const mss, trits_t buffer);

/**
 * Deerialize Merkle tree.
 *
 * @param mss [out] MSS interface
 * @param buffer [in] The serialized MT buffer
 *
 * @return void
 */
retcode_t mam_mss_deserialize(trits_t *buffer, mam_mss_t *mss);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_MSS_MSS_H__

/** @} */
