/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
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
#ifndef __MAM_MSS_MSS_TRAVERSAL_H__
#define __MAM_MSS_MSS_TRAVERSAL_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/mss/mss_common.h"
#include "mam/prng/prng.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"
#include "mam/wots/wots.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#define MAM_MSS_MT_MAX_STORED_SIZE(d) ((d) * ((d) + 3) / 2 * MAM_MSS_MT_HASH_SIZE)
#define MAM_MSS_MAX_STORED_SIZE(d) (4 + 14 + MAM_MSS_MT_MAX_STORED_SIZE(d))

/*! \brief MSS interface used to generate public key and sign. */
typedef struct mam_mss_s {
  mss_mt_height_t height; /*!< Merkle tree height. */
  mss_mt_idx_t skn;       /*!< Current WOTS private key number. */
  mam_prng_t *prng;       /*!< PRNG interface used to generate WOTS private keys. */
  trit_t *auth_path;      /*!< Current authentication path; `d` hash values. */
  trit_t *nodes_hashes; /*!< Buffer storing hash-values of auxiliary nodes; MAM_MSS_MT_NODES(d) hash-values in total. */
  mss_mt_node_t *nodes; /*<! Auxiliary node infos. */
  mss_mt_stack_t *stacks;                 /*<! Stacks used by traversal algorithm. */
  trits_t nonce1, nonce2, nonce3, nonce4; /*!< Nonce = `N1`||`N2`||`N3`||`N4`, stored pointers only, NOT copies. */
  trit_t root[MAM_MSS_PK_SIZE];
} mam_mss_t;

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

#ifdef __cplusplus
}
#endif

#endif  // __MAM_MSS_MSS_TRAVERSAL_H__
