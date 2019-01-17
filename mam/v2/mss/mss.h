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
\file mss.c
\brief MAM2 MSS layer.
*/
#ifndef __MAM_V2_MSS_MSS_H__
#define __MAM_V2_MSS_MSS_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/trits/trits.h"
#include "mam/v2/wots/wots.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Use Merkle tree traversal algorithm on MSS layer. */
#define MAM2_MSS_TRAVERSAL

/*! \brief Enable debug MT hash function to ease MT traversal debug. */
/*#define MAM2_MSS_DEBUG*/

/*! \brief MSS public key size. */
#define MAM2_MSS_PK_SIZE 243
/*! \brief Trits needed to encode `skn`: tree depth and key number. */
#define MAM2_MSS_SKN_SIZE 18
/*! \brief MSS authentication path size of height `d`. */
#define MAM2_MSS_APATH_SIZE(d) (MAM2_WOTS_PK_SIZE * d)
/*! \brief MSS signature size with a tree of height `d`. */
#define MAM2_MSS_SIG_SIZE(d) \
  (MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE + MAM2_MSS_APATH_SIZE(d))
/*! \brief MSS signed hash value size. */
#define MAM2_MSS_HASH_SIZE MAM2_WOTS_HASH_SIZE

/*! \brief Max Merkle tree height. */
#define MAM2_MSS_MAX_D 20

/*! \brief Size of hash values stored in Merkle tree */
#define MAM2_MSS_MT_HASH_SIZE MAM2_WOTS_PK_SIZE

/*! \brief Leaves have height `0`, root has height `D`; `0 <= d < D`; `D <= 20`.
 */
typedef trint6_t mss_mt_height_t;
/*! \brief Index (skn) of leaf/node on the level of height `d`; 0 <= i <
 * 2^(D-d). */
typedef trint18_t mss_mt_idx_t;

#if defined(MAM2_MSS_TRAVERSAL)

/*! \brief Node info, specifies position of the node in the MT.
\note Corresponding hash-value is stored externally. */
typedef struct mss_mt_node_s {
  mss_mt_height_t d; /*!< Height. */
  mss_mt_idx_t i;    /*!< Level index. */
} mss_mt_node_t;
/*! \brief Number of auxiliary MT nodes used by tree-traversal algorithm */
#define MAM2_MSS_MT_NODES(d) ((d) * ((d) + 1) / 2)

/*! \brief Stack info.
\note Stack nodes are stored externally. */
typedef struct mss_mt_stack_s {
  mss_mt_height_t d; /*!< Height. */
  mss_mt_idx_t i;    /*!< Level index. */
  size_t s;          /*!< Size of stack. */
} mss_mt_stack_t;
/*! \brief Number of auxiliary stacks used by tree-traversal algorithm */
#define MAM2_MSS_MT_STACKS(d) (d)
#define MAM2_MSS_MT_STACK_CAPACITY(d) ((d) + 1)

#define MAM2_MSS_HASH_IDX(i) (MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE) * (i))
/*! \brief Memory for hash-values. TODO: Add 1 extra hash for mss_gen. */
#define MAM2_MSS_MT_HASH_WORDS(d, i) \
  MAM2_MSS_HASH_IDX(MAM2_MSS_MT_NODES(d) + (i))
#define MAM2_MSS_MT_AUTH_WORDS(d) MAM2_MSS_HASH_IDX(d)
#else
/*! \brief MSS Merkle-tree implementation storage words. */
#define MAM2_MSS_MT_WORDS(d) \
  (MAM2_WORDS(MAM2_WOTS_PK_SIZE) * (2 * (1 << d) - 1))
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define MAM2_MSS_MT_MAX_STORED_SIZE(d) \
  ((d) * ((d) + 3) / 2 * MAM2_MSS_MT_HASH_SIZE)
#else
#define MAM2_MSS_MT_MAX_STORED_SIZE(d) \
  (((1 << ((d) + 1)) - 1) * MAM2_MSS_MT_HASH_SIZE)
#endif
#define MAM2_MSS_MAX_STORED_SIZE(d) (4 + 14 + MAM2_MSS_MT_MAX_STORED_SIZE(d))

/*! \brief MSS interface used to generate public key and sign. */
typedef struct mss_s {
  trint6_t d;      /*!< Merkle tree height. */
  trint18_t skn;   /*!< Current WOTS private key number. */
  prng_t *p;       /*!< PRNG interface used to generate WOTS private keys. */
  spongos_t sg[1]; /*!< Spongos interface used to hash Merkle tree nodes. */
  wots_t *w;       /*!< WOTS interface used to generate keys and sign. */
#if defined(MAM2_MSS_TRAVERSAL)
  word_t *ap;         /*!< Current authentication path; `d` hash values. */
  word_t *hs;         /*!< Buffer storing hash-values of auxiliary nodes;
                      MAM2_MSS_MT_NODES(d) hash-values in total. */
  mss_mt_node_t *ns;  /*<! Auxiliary node infos. */
  mss_mt_stack_t *ss; /*<! Stacks used by traversal algorithm. */
#else
  word_t *mt; /*!< Buffer storing complete Merkle-tree. */
#endif
  trits_t N1, N2; /*!< Nonce = `N1`||`N2`, stored pointers only, NOT copies. */
#if defined(MAM2_MSS_DEBUG)
  size_t gen_leaf_count, hash_node_count;
#endif
} mss_t;

#if defined(MAM2_MSS_TRAVERSAL)
/*! \note
MSS key gen requires stack of capacity `D+1`.
In order to save space stack at level `D-1` is reused.
Although stack at level `d` has capacity `d+1`,
for the purpose of key gen stack `D-1` must have capacity `D+1`.
It is achieved by allocating one extra node:

```
  word_t *alloc_words(size_t word_count);
  mss_mt_node_t *alloc_nodes(size_t node_count);
  mss_mt_stack_t *alloc_stacks(size_t stack_count);
  mss_mt_height_t D;

  size_t total_nodes = MAM2_MSS_MT_NODES(D)+1;
  m->d = D;
  m->ap = alloc_words(D * MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE));
  m->hs = alloc_words(total_nodes * MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE));
  m->ns = alloc_nodes(total_nodes);
  m->ss = alloc_stacks(D);
```
*/
#endif

/*! \brief MSS interface initialization.
\note MSS Merkle tree should already be allocated
  and initialized for the current height.
*/
void mss_init(mss_t *m,    /*!< [in] MSS interface */
              prng_t *p,   /*!< [in] PRNG interface */
              sponge_t *s, /*!< [in] Sponge interface */
              wots_t *w,   /*!< [in] WOTS interface */
              trint6_t d,  /*!< [in] Merkle-tree height */
              trits_t N1,  /*!< [in] first nonce */
              trits_t N2   /*!< [in] second nonce */
);

/*! \brief Generate MSS keys. */
void mss_gen(mss_t *m,  /*!< [in] MSS interface */
             trits_t pk /*!< [out] public key, Merkle-tree root */
);

/*! \brief Encode MSS `skn`. */
void mss_skn(
    mss_t *m,   /*!< [in] MSS interface */
    trits_t skn /*!< [out] encoded height and current private key number */
);

/*! \brief Encode MSS authentication path. */
void mss_apath(mss_t *m,    /*!< [in] MSS interface */
               trint18_t i, /*!< [in] number of WOTS instance */
               trits_t p    /*!< [out] authentication path */
);

/*! \brief Generate MSS signature. */
void mss_sign(mss_t *m,   /*!< [in] MSS interface */
              trits_t H,  /*!< [in] hash value tbs */
              trits_t sig /*!< [out] signature */
);

/*! \brief Verify MSS signature. */
bool_t mss_verify(
    spongos_t *ms, /*!< [in] Spongos interface to hash Merkle Tree */
    spongos_t *ws, /*!< [in] Spongos interface to hash WOTS iterations */
    trits_t H,     /*!< [in] signed hash value */
    trits_t sig,   /*!< [in] signature */
    trits_t pk     /*!< [in] public key (Merkle-tree root) */
);

/*! \brief Allocate memory for internal Merkle tree structure.
\note `mss_init` must still be called afterwards.
\note In case of error `mss_destroy` must be called.
\note Non Merkle tree related objects (WOTS, PRNG, Spongos interfaces)
      must be allocated separately.
*/
retcode_t mss_create(mss_t *m, mss_mt_height_t d);

/*! \brief Deallocate memory for internal Merkle tree structure.
\note Pointer `m` must be freed afterwards.
*/
void mss_destroy(mss_t *m);

size_t mss_stored_size(mss_t *m);

/*! \brief Serialize Merkle tree. */
void mss_save(mss_t *m, trits_t b);

/*! \brief Deserialize Merkle tree. */
retcode_t mss_load(mss_t *m, trits_t *b);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MSS_MSS_H__
