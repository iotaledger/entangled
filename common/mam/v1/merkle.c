#include <stdio.h>

#include "common/mam/v1/merkle.h"
#include "common/sign/v2/iss_curl.h"

static trit_t const merkle_null_hash[HASH_LENGTH] = {0};

static size_t binary_tree_size(size_t acc, size_t depth) {
  if (depth == 0) {
    return acc + 1;
  }
  return binary_tree_size(acc + (1 << depth), depth - 1);
}

size_t merkle_size(size_t leaf_count) {
  size_t acc = 1;

  if (leaf_count == 0)
    return 0;
  else if (leaf_count == 1)
    return 1;
  while (leaf_count >= 2) {
    acc += leaf_count;
    leaf_count = (leaf_count + 1) >> 1;
  }
  return acc;
}

size_t merkle_depth(size_t node_count) {
  size_t depth = 0;

  while (binary_tree_size(0, depth) < node_count) {
    depth++;
  }
  return depth + 1;
}

static size_t merkle_node_index_traverse(size_t acc, size_t depth, size_t width,
                                         size_t tree_depth) {
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

size_t merkle_node_index(size_t depth, size_t width, size_t tree_depth) {
  return merkle_node_index_traverse(0, depth, width, tree_depth);
}

size_t merkle_leaf_index(size_t leaf_index, size_t leaf_count) {
  return leaf_index = leaf_count - leaf_index - 1;
}

int merkle_create(trit_t *const tree, trit_t *const seed, int64_t offset,
                  const size_t base_size, size_t security, Curl *const c) {
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
    iss_curl_address(&tree[HASH_LENGTH * merkle_node_index(td, leaf_index, td)],
                     key, HASH_LENGTH, c);
  }

  // hash tree
  for (size_t cur_size = base_size, depth = td; depth > 0; depth--) {
    for (size_t width = 0; width < cur_size; width += 2) {
      // if right hash exists, absorb right hash then left hash
      if (width < cur_size - 1) {
        curl_absorb(
            c, &tree[HASH_LENGTH * merkle_node_index(depth, width + 1, td)],
            HASH_LENGTH);
        curl_absorb(c, &tree[HASH_LENGTH * merkle_node_index(depth, width, td)],
                    HASH_LENGTH);
      }
      // else, absorb the remaining hash then a null hash
      else {
        curl_absorb(c, &tree[HASH_LENGTH * merkle_node_index(depth, width, td)],
                    HASH_LENGTH);
        curl_absorb(c, (trit_t *)merkle_null_hash, HASH_LENGTH);
      }
      // squeeze the result in the parent node
      curl_squeeze(
          c, &tree[HASH_LENGTH * merkle_node_index(depth - 1, width / 2, td)],
          HASH_LENGTH);
      curl_reset(c);
    }
    cur_size = (cur_size + 1) >> 1;
  }
  return 0;
}

int merkle_branch(trit_t *siblings, trit_t *const merkle_tree,
                  size_t tree_length, size_t tree_depth, size_t leaf_index,
                  size_t leaf_count) {
  if (siblings == NULL) {
    return NULL_SIBLING;
  }
  if (merkle_tree == NULL) {
    return NULL_TREE;
  }
  if (HASH_LENGTH *
          merkle_node_index(tree_depth - 1, leaf_index, tree_depth - 1) >=
      tree_length) {
    return LEAF_INDEX_OUT_OF_BOUNDS;
  }
  if (tree_depth > merkle_depth(tree_length / HASH_LENGTH))
    return DEPTH_OUT_OF_BOUNDS;

  size_t depth_index, sibling_index, site_index;

  leaf_index = merkle_leaf_index(leaf_index, leaf_count);

  if (leaf_index & 1) {
    sibling_index = leaf_index - 1;
  } else {
    sibling_index = leaf_index + 1;
  }

  for (depth_index = tree_depth - 1; depth_index > 0; depth_index--) {
    site_index = HASH_LENGTH *
                 merkle_node_index(depth_index, sibling_index, tree_depth - 1);
    if (site_index >= tree_length) {
      // if depth width is not even, copy a null hash
      memcpy(siblings, merkle_null_hash, HASH_LENGTH * sizeof(trit_t));
    } else {
      // else copy a sibling
      memcpy(siblings, &merkle_tree[site_index], HASH_LENGTH * sizeof(trit_t));
    }

    sibling_index >>= 1;
    if (sibling_index & 1) {
      sibling_index--;
    } else {
      sibling_index++;
    }
    siblings += HASH_LENGTH;
  }
  return 0;
}

int merkle_root(trit_t *hash, trit_t *siblings, size_t siblings_number,
                size_t leaf_index, Curl *c) {
  for (size_t i = 0; i < siblings_number; i++) {
    // if index is a right node, absorb a sibling (left) then the hash
    if (leaf_index & 1) {
      curl_absorb(c, siblings, HASH_LENGTH);
      curl_absorb(c, hash, HASH_LENGTH);
    }
    // if index is a left node, absorb the hash then a sibling (right)
    else {
      curl_absorb(c, hash, HASH_LENGTH);
      curl_absorb(c, siblings, HASH_LENGTH);
    }
    curl_squeeze(c, hash, HASH_LENGTH);
    curl_reset(c);
    leaf_index >>= 1;
    siblings += HASH_LENGTH;
  }
  return 0;
}
