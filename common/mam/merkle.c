#include "mam/merkle.h"
#include "common/sign/v2/iss_curl.h"

static trit_t const merkle_null_hash[HASH_LENGTH] = {0};

// thanks to
// https://stackoverflow.com/questions/3272424/compute-fast-log-base-2-ceiling#3272516
static int ceil_log2(unsigned long long x) {
  static const unsigned long long t[6] = {
      0xFFFFFFFF00000000ull, 0x00000000FFFF0000ull, 0x000000000000FF00ull,
      0x00000000000000F0ull, 0x000000000000000Cull, 0x0000000000000002ull};

  int y = (((x & (x - 1)) == 0) ? 0 : 1);
  int j = 32;
  int depth_i;

  for (depth_i = 0; depth_i < 6; depth_i++) {
    int k = (((x & t[depth_i]) == 0) ? 0 : j);
    y += k;
    x >>= k;
    j >>= 1;
  }

  return y;
}

static size_t binary_tree_size(size_t depth, size_t acc) {
  if (depth == 0) {
    return acc + 1;
  }
  return binary_tree_size(depth - 1, acc + (1 << depth));
}

static size_t merkle_node_base_index(size_t acc, size_t depth, size_t width,
                                     size_t base_size) {
  if (depth == 0) return acc;
  acc += width + 1;
  if (base_size % 2 == 0)
    width = width / 2;
  else
    width = (width % 2 == 0 ? width / 2 : (width + 1) / 2);
  return merkle_node_base_index(acc, depth - 1, width, (base_size + 1) / 2);
}

size_t merkle_node_index(size_t depth, size_t width, size_t base_size,
                         size_t tree_depth) {
  size_t acc = 0;
  size_t idx = width;

  while (depth < tree_depth) {
    if (width != 0) idx = idx * 2 - 1;
    acc--;
    depth++;
  }
  return acc + merkle_node_base_index(0, tree_depth, idx, base_size);
}

size_t merkle_size(size_t leaf_count) {
  size_t acc = 1;

  while (leaf_count >= 2) {
    acc += leaf_count;
    leaf_count = (leaf_count + 1) >> 1;
  }
  return acc;
}

size_t merkle_depth(size_t node_count) {
  size_t depth = 1;

  while (binary_tree_size(depth, 0) < node_count) {
    depth++;
  }
  return depth + 1;
}

int merkle_create(trit_t *const tree, trit_t *const seed, int64_t offset,
                  const size_t base_size, size_t key_size, Curl *const c) {
  size_t depth_i, e;
  ssize_t j;
  trit_t key[key_size];

  const size_t td = ceil_log2(base_size);

  // create addresses
  for (depth_i = 0; depth_i < base_size; depth_i++) {
    iss_curl_subseed(seed, key, offset + depth_i, c);
    iss_curl_key(key, key, key_size, c);
    iss_curl_key_digest(key, key, key_size, c);
    iss_curl_address(
        &tree[HASH_LENGTH * merkle_node_index(td, depth_i, base_size, td)], key,
        HASH_LENGTH, c);
  }
  curl_reset(c);

  // hash tree
  for (e = base_size, depth_i = td; depth_i > 0; depth_i--, e = e / 2 + e % 2) {
    for (j = 0; j < e; j += 2) {
      // absorb left hash
      curl_absorb(
          c, &tree[HASH_LENGTH * merkle_node_index(depth_i, j, base_size, td)],
          HASH_LENGTH);
      // if number of leaves is odd, absorb a null hash as right node
      if (j == 0 && e % 2 == 1) {
        memset(c->state, 0, HASH_LENGTH * sizeof(trit_t));
        curl_absorb(c, (trit_t *)merkle_null_hash, HASH_LENGTH);
        j -= 1;
      }
      // else aborb right hash
      else {
        curl_absorb(c,
                    &tree[HASH_LENGTH *
                          merkle_node_index(depth_i, j + 1, base_size, td)],
                    HASH_LENGTH);
      }
      // squeeze the result in the parent node
      curl_squeeze(
          c,
          &tree[HASH_LENGTH *
                merkle_node_index(depth_i - 1, (j + 1) / 2, base_size, td)],
          HASH_LENGTH);
      curl_reset(c);
    }
  }
  return 0;
}

// // needs tests
// int merkle_root(trit_t *hash, trit_t *sibling, size_t depth, size_t index,
//                 Curl *c) {
//   trit_t *siblings_end = &sibling[depth * HASH_LENGTH];
//   for (; sibling < siblings_end; sibling = &sibling[HASH_LENGTH]) {
//     if (index & 1) {
//       curl_absorb(c, sibling, HASH_LENGTH);
//       curl_absorb(c, hash, HASH_LENGTH);
//     } else {
//       curl_absorb(c, hash, HASH_LENGTH);
//       curl_absorb(c, sibling, HASH_LENGTH);
//     }
//     curl_squeeze(c, hash, HASH_LENGTH);
//     curl_reset(c);
//     index >>= 1;
//   }
//   return 0;
// }

// int merkle_branch(trit_t *sibling, trit_t *const merkle_tree,
//                   size_t tree_length, size_t tree_depth, size_t leaf_index) {
//   if (sibling == NULL) {
//     return NULL_SIBLING;
//   }
//   if (merkle_tree == NULL) {
//     return NULL_TREE;
//   }
//   // TODO: Fix these
//   // if (HASH_LENGTH * merkle_node_index(tree_depth, leaf_index, tree_depth)
//   >=
//   //     tree_length) {
//   //   return LEAF_INDEX_OUT_OF_BOUNDS;
//   // }
//   // if (HASH_LENGTH * merkle_node_index(tree_depth, 0, tree_depth) >=
//   //     tree_length) {
//   //   return DEPTH_OUT_OF_BOUNDS;
//   // }
//
//   size_t depth_i, sib_i, site_i;
//
//   if (leaf_index & 1) {
//     sib_i = leaf_index - 1;
//   } else {
//     sib_i = leaf_index + 1;
//   }
//
//   for (depth_i = tree_depth; depth_i > 0; depth_i--) {  // append siblings
//     site_i = HASH_LENGTH * merkle_node_index(depth_i, sib_i, tree_depth);
//
//     if (site_i >= tree_length) {
//       // Copy null hash in case where num. of leaves is not a power of 2
//       memset(sibling, 0, HASH_LENGTH * sizeof(trit_t));
//     } else {
//       memcpy(sibling, &merkle_tree[site_i], HASH_LENGTH * sizeof(trit_t));
//     }
//
//     sib_i >>= 1;
//
//     if (sib_i & 1) {
//       sib_i--;
//     } else {
//       sib_i++;
//     }
//
//     sibling = &sibling[HASH_LENGTH];
//   }
//   return 0;
// }
