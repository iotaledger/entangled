#include "mam/merkle.h"
#include "common/sign/v1/iss_curl.h"

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
    return 2;
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

int merkle_create(trit_t *const tree, trit_t *const seed, int64_t offset,
                  const size_t base_size, size_t security, Curl *const c) {
  size_t depth_i;
  size_t key_size = security * ISS_KEY_LENGTH;
  trit_t key[key_size];

  const size_t td = merkle_depth(merkle_size(base_size)) - 1;

  // create base addresses
  for (depth_i = 0; depth_i < base_size; depth_i++) {
    iss_curl_subseed(seed, key, offset + depth_i, c);
    iss_curl_key(key, key, key_size, c);
    iss_curl_key_digest(key, key, key_size, c);
    iss_curl_address(key,
                     &tree[HASH_LENGTH * merkle_node_index(td, depth_i, td)],
                     HASH_LENGTH, c);
  }
  curl_reset(c);

  // hash tree
  for (size_t cur_size = base_size, depth_i = td; depth_i > 0; depth_i--) {
    for (size_t j = 0; j < cur_size; j += 2) {
      // absorb left hash
      curl_absorb(c, &tree[HASH_LENGTH * merkle_node_index(depth_i, j, td)],
                  HASH_LENGTH);
      // if depth width is odd, absorb a null hash as right hash
      if (j + 1 >= cur_size) {
        curl_absorb(c, (trit_t *)merkle_null_hash, HASH_LENGTH);
      }
      // else absorb right hash
      else {
        curl_absorb(c,
                    &tree[HASH_LENGTH * merkle_node_index(depth_i, j + 1, td)],
                    HASH_LENGTH);
      }
      // squeeze the result in the parent node
      curl_squeeze(
          c, &tree[HASH_LENGTH * merkle_node_index(depth_i - 1, j / 2, td)],
          HASH_LENGTH);
      curl_reset(c);
    }
    cur_size = (cur_size + 1) >> 1;
  }
  return 0;
}

int merkle_branch(trit_t *sibling, trit_t *const merkle_tree,
                  size_t tree_length, size_t tree_depth, size_t leaf_index) {
  if (sibling == NULL) {
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

  size_t depth_i, sib_i, site_i;

  if (leaf_index & 1) {
    sib_i = leaf_index - 1;
  } else {
    sib_i = leaf_index + 1;
  }

  for (depth_i = tree_depth - 1; depth_i > 0; depth_i--) {
    site_i = HASH_LENGTH * merkle_node_index(depth_i, sib_i, tree_depth - 1);
    if (site_i >= tree_length) {
      // if depth width is not even, copy a null hash
      memcpy(sibling, merkle_null_hash, HASH_LENGTH * sizeof(trit_t));
    } else {
      // else copy a sibling
      memcpy(sibling, &merkle_tree[site_i], HASH_LENGTH * sizeof(trit_t));
    }

    sib_i >>= 1;
    if (sib_i & 1) {
      sib_i--;
    } else {
      sib_i++;
    }
    sibling += HASH_LENGTH;
  }
  return 0;
}

int merkle_root(trit_t *hash, trit_t *siblings, size_t siblings_number,
                size_t index, Curl *c) {
  for (size_t i = 0; i < siblings_number; i++) {
    if (index & 1) {
      curl_absorb(c, siblings, HASH_LENGTH);
      curl_absorb(c, hash, HASH_LENGTH);
    } else {
      curl_absorb(c, hash, HASH_LENGTH);
      curl_absorb(c, siblings, HASH_LENGTH);
    }
    curl_squeeze(c, hash, HASH_LENGTH);
    curl_reset(c);
    index >>= 1;
    siblings += HASH_LENGTH;
  }
  return 0;
}
