/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_MSS_H__
#define __MAM_V2_MSS_H__

#include "mam/v2/defs.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/wots.h"
#include "trits.h"

// Use Merkle tree traversal algorithm on MSS layer
#define MAM2_MSS_TRAVERSAL

// Enable debug MT hash function to ease MT traversal debug
//#define MAM2_MSS_DEBUG

// MSS public key size
#define MAM2_MSS_PK_SIZE 243
// Trits needed to encode SKN (tree depth and key number)
#define MAM2_MSS_SKN_SIZE 18
// Maximum value of SKN for a given height d
#define MAM2_MSS_MAX_SKN(d) (((uint32_t)1 << d) - 1)
// MSS authentication path size of height d
#define MAM2_MSS_AUTH_PATH_SIZE(d) (MAM2_WOTS_PK_SIZE * d)
// MSS signature size with a tree of height d
#define MAM2_MSS_SIG_SIZE(d) \
  (MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE + MAM2_MSS_AUTH_PATH_SIZE(d))
// MSS signed hash value size
#define MAM2_MSS_HASH_SIZE MAM2_WOTS_HASH_SIZE
// Max Merkle tree height
#define MAM2_MSS_MAX_HEIGHT 20
// Size of hash values stored in Merkle tree
#define MAM2_MSS_MT_HASH_SIZE MAM2_WOTS_PK_SIZE

// Leaves have height `0`, root has height `D`; `0 <= d < D`; `D <= 20`
typedef uint8_t mss_mt_height_t;
// Index (SKN) of leaf/node on the level of height `d`; 0 <= i < 2^(D-d)
typedef uint32_t mss_mt_index_t;

#if defined(MAM2_MSS_TRAVERSAL)

/**
 * Node info, specifies position of the node in the MT
 * Corresponding hash-value is stored externally
 */
typedef struct mss_mt_node_s {
  mss_mt_height_t height;
  mss_mt_index_t index;
} mss_mt_node_t;

// Number of auxiliary MT nodes used by tree-traversal algorithm
#define MAM2_MSS_MT_NODES(d) ((d) * ((d) + 1) / 2)

/**
 * Stack info
 * Stack nodes are stored externally
 */
typedef struct mss_mt_stack_s {
  mss_mt_height_t height;
  mss_mt_index_t index;
  size_t stack_size;
} mss_mt_stack_t;

// Number of auxiliary stacks used by tree-traversal algorithm
#define MAM2_MSS_MT_STACKS(d) (d)
#define MAM2_MSS_MT_STACK_CAPACITY(d) ((d) + 1)
#define MAM2_MSS_HASH_IDX(i) (MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE) * (i))
// Memory for hash-values. TODO: Add 1 extra hash for mss_gen. */
#define MAM2_MSS_MT_HASH_WORDS(d, i) \
  MAM2_MSS_HASH_IDX(MAM2_MSS_MT_NODES(d) + (i))
#define MAM2_MSS_MT_AUTH_WORDS(d) MAM2_MSS_HASH_IDX(d)
#else
// MSS Merkle-tree implementation storage words
#define MAM2_MSS_MT_WORDS(d) \
  (MAM2_WORDS(MAM2_WOTS_PK_SIZE) * (2 * (1 << d) - 1))
#endif

// MSS interface used to generate public key and sign
typedef struct mss_s {
  mss_mt_height_t height;  // Merkle tree height
  mss_mt_index_t skn;      // Current WOTS private key number
  iprng *prng;             // PRNG interface used to generate WOTS private keys
  isponge *sponge;         // Sponge interface used to hash Merkle tree nodes
  iwots *wots;             // WOTS interface used to generate keys and sign
#if defined(MAM2_MSS_TRAVERSAL)
  trit_t *auth_path;       // Current authentication path; `d` hash values
  trit_t *hashes;          // Buffer storing hash-values of auxiliary nodes;
                           // MAM2_MSS_MT_NODES(d) hash-values in total
  mss_mt_node_t *nodes;    // Auxiliary node infos
  mss_mt_stack_t *stacks;  // Stacks used by traversal algorithm
#else
  trit_t *merkle_tree;  // Buffer storing complete Merkle-tree
#endif
  // Nonce = `nonce1`||`nonce2`, stored pointers only, NOT copies
  trits_t nonce1;
  trits_t nonce2;
} mss_t;

#if defined(MAM2_MSS_TRAVERSAL)

/**
 * MSS key gen requires stack of capacity `D+1`.
 * In order to save space stack at level `D-1` is reused.
 * Although stack at level `d` has capacity `d+1`,
 * for the purpose of key gen stack `D-1` must have capacity `D+1`.
 * It is achieved by allocating one extra node:
 *
 * ```
 *  trit_t *alloc_words(size_t word_count);
 *  mss_mt_node *alloc_nodes(size_t node_count);
 *  mss_mt_stack *alloc_stacks(size_t stack_count);
 *  mss_mt_height_t D;
 *
 *  size_t total_nodes = MAM2_MSS_MT_NODES(D)+1;
 *  m->d = D;
 *  m->ap = alloc_words(D * MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE));
 *  m->hs = alloc_words(total_nodes * MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE));
 *  m->ns = alloc_nodes(total_nodes);
 *  m->ss = alloc_stacks(D);
 * ```
 */

#endif

/**
 * MSS interface initialization
 * MSS Merkle tree should already be allocated and initialized for the current
 * height
 *
 * @param mss MSS interface
 * @param prng PRNG interface
 * @param sponge Sponge interface
 * @param wots WOTS interface
 * @param height Merkle-tree height
 * @param nonce1 First nonce
 * @param nonce2 Second nonce
 */
void mss_init(mss_t *const mss, iprng *const prng, isponge *const sponge,
              iwots *const wots, mss_mt_height_t const height,
              trits_t const nonce1, trits_t const nonce2);

/**
 * Generates MSS keys
 *
 * @param mss MSS interface
 * @param pk public key, Merkle-tree root
 */
void mss_gen(mss_t *mss, trits_t pk);

/**
 * Encode MSS SKN
 *
 * @param mss MSS interface
 * @param skn Encoded height and current private key number
 */
void mss_skn(mss_t *mss, trits_t skn);

/**
 * Encodes MSS authentication path
 *
 * @param mss MSS interface
 * @param i Number of WOTS instance
 * @param auth_path Authentication path
 */
void mss_auth_path(mss_t *mss, trint18_t i, trits_t auth_path);

/**
 * Generates MSS signature
 *
 * @param mss MSS interface
 * @param hash Hash value to be signed
 * @param sig Signature
 */
void mss_sign(mss_t *mss, trits_t hash, trits_t sig);

/**
 * Verifies MSS signature
 *
 * @param ms Sponge interface used to hash Merkle Tree
 * @param ws Sponge interface used to
 * @param hash Signed hash value
 * @param sig Signature
 * @param pk public key (Merkle-tree root)
 */
bool_t mss_verify(isponge *ms, isponge *ws, trits_t hash, trits_t sig,
                  trits_t pk);

/**
 * Allocate memory for internal Merkle tree structure
 * `mss_init` must still be called afterwards
 * In case of error `mss_destroy` must be called
 * Non Merkle tree related objects (WOTS, PRNG, Sponge interfaces)
 * must be allocated separately
 *
 * @param mss MSS interface
 * @param height Merkle-tree height
 */
err_t mss_create(mss_t *mss, mss_mt_height_t height);

/**
 * Deallocate memory for internal Merkle tree structure
 * Pointer `mss` must be freed afterwards
 *
 * @param mss MSS interface
 */
void mss_destroy(mss_t *mss);

#endif  // __MAM_V2_MSS_H__
