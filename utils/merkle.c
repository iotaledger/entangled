/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "common/sign/v2/iss_curl.h"
#include "utils/merkle.h"

static trit_t const merkle_null_hash[HASH_LENGTH_TRIT] = {0};

static size_t binary_tree_size(size_t const acc, size_t const depth) {
  if (depth == 0) {
    return acc + 1;
  }
  return binary_tree_size(acc + (1 << depth), depth - 1);
}

size_t merkle_size(size_t const leaf_count) {
  size_t acc = 1, count = leaf_count;

  if (count == 0)
    return 0;
  else if (count == 1)
    return 1;
  while (count >= 2) {
    acc += count;
    count = (count + 1) >> 1;
  }
  return acc;
}

size_t merkle_depth(size_t const node_count) {
  size_t depth = 0;

  while (binary_tree_size(0, depth) < node_count) {
    depth++;
  }
  return depth + 1;
}

static size_t merkle_node_index_traverse(size_t const acc, size_t const depth,
                                         size_t const width,
                                         size_t const tree_depth) {
  if (depth == 0) return acc;
  if (width < (1 << (depth - 1))) {
    return merkle_node_index_traverse(acc + 1, depth - 1, width,
                                      tree_depth - 1);
  } else {
    return merkle_node_index_traverse(
        1 + acc + binary_tree_size(0, tree_depth - 1), depth - 1,
        width - (1 << (depth - 1)), tree_depth - 1);
  }
}

size_t merkle_node_index(size_t const depth, size_t const width,
                         size_t const tree_depth) {
  return merkle_node_index_traverse(0, depth, width, tree_depth);
}

size_t merkle_leaf_index(size_t const leaf_index, size_t const leaf_count) {
  return leaf_count - leaf_index - 1;
}

int merkle_create(trit_t *const tree, size_t const base_size,
                  trit_t const *const seed, int64_t const offset,
                  size_t const security, Curl *const c) {
  size_t key_size = security * ISS_KEY_LENGTH;
  trit_t key[key_size];

  // enforcing the tree to be perfect by checking if the base size (number of
  // leaves) is a power of two
  if ((base_size != 0) && (base_size & (base_size - 1)) != 0) {
    fprintf(stderr, "base size of the merkle tree should be a power of 2\n");
    return -1;
  }

  const size_t td = merkle_depth(merkle_size(base_size)) - 1;

  // create base addresses
  for (size_t leaf_index = 0; leaf_index < base_size; leaf_index++) {
    iss_curl_subseed(seed, key,
                     offset + merkle_leaf_index(leaf_index, base_size), c);
    iss_curl_key(key, key, key_size, c);
    iss_curl_key_digest(key, key, key_size, c);
    iss_curl_address(
        &tree[HASH_LENGTH_TRIT * merkle_node_index(td, leaf_index, td)], key,
        HASH_LENGTH_TRIT, c);
  }

  // hash tree
  for (size_t cur_size = base_size, depth = td; depth > 0; depth--) {
    for (size_t width = 0; width < cur_size; width += 2) {
      // if right hash exists, absorb right hash then left hash
      if (width < cur_size - 1) {
        curl_absorb(
            c,
            &tree[HASH_LENGTH_TRIT * merkle_node_index(depth, width + 1, td)],
            HASH_LENGTH_TRIT);
        curl_absorb(
            c, &tree[HASH_LENGTH_TRIT * merkle_node_index(depth, width, td)],
            HASH_LENGTH_TRIT);
      }
      // else, absorb the remaining hash then a null hash
      else {
        curl_absorb(
            c, &tree[HASH_LENGTH_TRIT * merkle_node_index(depth, width, td)],
            HASH_LENGTH_TRIT);
        curl_absorb(c, (trit_t *)merkle_null_hash, HASH_LENGTH_TRIT);
      }
      // squeeze the result in the parent node
      curl_squeeze(
          c,
          &tree[HASH_LENGTH_TRIT * merkle_node_index(depth - 1, width / 2, td)],
          HASH_LENGTH_TRIT);
      curl_reset(c);
    }
    cur_size = (cur_size + 1) >> 1;
  }
  return 0;
}

int merkle_branch(trit_t const *const tree, trit_t *const siblings,
                  size_t const tree_length, size_t const tree_depth,
                  size_t const leaf_index, size_t const leaf_count) {
  if (siblings == NULL) {
    return NULL_SIBLING;
  }
  if (tree == NULL) {
    return NULL_TREE;
  }
  if (HASH_LENGTH_TRIT *
          merkle_node_index(tree_depth - 1, leaf_index, tree_depth - 1) >=
      tree_length) {
    return LEAF_INDEX_OUT_OF_BOUNDS;
  }
  if (tree_depth > merkle_depth(tree_length / HASH_LENGTH_TRIT))
    return DEPTH_OUT_OF_BOUNDS;

  size_t sibling_index, site_index;

  sibling_index = merkle_leaf_index(leaf_index, leaf_count);

  for (size_t i = 0, depth_index = tree_depth - 1; depth_index > 0;
       i++, depth_index--) {
    if (sibling_index & 1) {
      sibling_index--;
    } else {
      sibling_index++;
    }
    site_index = HASH_LENGTH_TRIT *
                 merkle_node_index(depth_index, sibling_index, tree_depth - 1);
    if (site_index >= tree_length) {
      // if depth width is not even, copy a null hash
      memcpy(&siblings[i * HASH_LENGTH_TRIT], merkle_null_hash,
             HASH_LENGTH_TRIT * sizeof(trit_t));
    } else {
      // else copy a sibling
      memcpy(&siblings[i * HASH_LENGTH_TRIT], &tree[site_index],
             HASH_LENGTH_TRIT * sizeof(trit_t));
    }

    sibling_index >>= 1;
  }
  return 0;
}

void merkle_root(trit_t *const hash, trit_t const *const siblings,
                 size_t const siblings_number, size_t const leaf_index,
                 Curl *const c) {
  for (size_t i = 0, j = 1; i < siblings_number; i++, j <<= 1) {
    // if index is a right node, absorb a sibling (left) then the hash
    if (leaf_index & j) {
      curl_absorb(c, &siblings[i * HASH_LENGTH_TRIT], HASH_LENGTH_TRIT);
      curl_absorb(c, hash, HASH_LENGTH_TRIT);
    }
    // if index is a left node, absorb the hash then a sibling (right)
    else {
      curl_absorb(c, hash, HASH_LENGTH_TRIT);
      curl_absorb(c, &siblings[i * HASH_LENGTH_TRIT], HASH_LENGTH_TRIT);
    }
    curl_squeeze(c, hash, HASH_LENGTH_TRIT);
    curl_reset(c);
  }
}
