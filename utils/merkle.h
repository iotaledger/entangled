/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_MERKLE_H__
#define __UTILS_MERKLE_H__

#include "common/curl-p/trit.h"

#define NULL_SIBLING 1
#define NULL_TREE 2
#define LEAF_INDEX_OUT_OF_BOUNDS 3
#define DEPTH_OUT_OF_BOUNDS 4

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Computes the size of a merkle tree, e.g. its node number
 *
 * @param leaf_count The number of leaves of the tree
 *
 * @return The size of the merkle tree
 */
size_t merkle_size(size_t const leaf_count);

/**
 * Computes the depth of a merkle tree
 *
 * @param node_count The number of nodes of the tree
 *
 * @return The depth of the merkle tree
 */
size_t merkle_depth(size_t const node_count);

/**
 * Indexes a given node in the tree
 *
 * @param depth The depth of the node
 * @param width The width of the node
 * @param tree_depth The depth of the tree
 *
 * @return The index of the node
 */
size_t merkle_node_index(size_t const depth, size_t const width,
                         size_t const tree_depth);

/**
 * Helper function to compute the actual site index of a leaf
 *
 * @param leaf_index The leaf index
 * @param leaf_count The number of leaves
 *
 * @return The leaf site index
 */
size_t merkle_leaf_index(size_t const leaf_index, size_t const leaf_count);

/**
 * Creates a merkle tree
 *
 * @param tree The tree - Must be allocated
 * @param base_size Base size of the tree, e.g. the number of leaves
 * @param seed Seed used to generate addresses - Not sent over the network
 * @param offset Offset used to generate addresses
 * @param security Security used to generate addresses
 * @param c Curl instance used to generate addresses
 *
 * @return Success/error code
 */
int merkle_create(trit_t *const tree, size_t const base_size,
                  trit_t const *const seed, int64_t const offset,
                  size_t const security, Curl *const c);

/**
 * Creates the merkle branch to generate back root from index
 *
 * @param tree The merkle tree
 * @param siblings The siblings of the indexed node - Must be allocated
 * @param tree_length The length of the tree
 * @param tree_depth The depth of the tree
 * @param leaf_index The index of the leaf to start the branch from
 * @param leaf_count The number of leaves of the tree
 *
 * @return Success/error code
 */
int merkle_branch(trit_t const *const tree, trit_t *const siblings,
                  size_t const tree_length, size_t const tree_depth,
                  size_t const leaf_index, size_t const leaf_count);

/**
 * Generates a merkle root from a hash and his siblings
 *
 * @param hash The hash
 * @param siblings The hash siblings
 * @param siblings_number The number of siblings
 * @param leaf_index The node index of the hash
 * @param c A curl instance to compute hashes
 */
void merkle_root(trit_t *const hash, trit_t const *const siblings,
                 size_t const siblings_number, size_t const leaf_index,
                 Curl *const c);

#ifdef __cplusplus
}
#endif

#endif  //__UTILS_MERKLE_H__
