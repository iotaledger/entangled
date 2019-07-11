/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/protocol/handshake.h"

int handshake_supported_version(uint8_t const* const own_supported_versions, size_t const own_supported_versions_length,
                                uint8_t const* const supported_versions, size_t const supported_versions_length) {
  int highest_supported_version = 0;

  for (size_t i = 0; i < own_supported_versions_length; i++) {
    // max check up to advertised versions by the neighbor
    if (i > supported_versions_length - 1) {
      break;
    }

    // get versions matched by both
    int supported = (uint8_t)(supported_versions[i] & own_supported_versions[i]);

    // none supported
    if (supported == 0) {
      continue;
    }

    // iterate through all bits and find highest (more to the left is higher)
    int highest = 0;
    for (int j = 0; j < 8; j++) {
      if (((supported >> j) & 1) == 1) {
        highest = j + 1;
      }
    }
    highest_supported_version = highest + (i * 8);
  }

  // if the highest version is still 0, it means that we don't support any protocol version the neighbor supports
  if (highest_supported_version == 0) {
    // grab last byte denoting the highest versions, a node will only hold version bytes if at least one version in that
    // byte is supported, therefore it's safe to assume, that the last byte contains the highest supported version of a
    // given node.
    int last_versions_byte = supported_versions[supported_versions_length - 1];
    // find highest version
    int highest = 0;
    for (int j = 0; j < 8; j++) {
      if (((last_versions_byte >> j) & 1) == 1) {
        highest = j + 1;
      }
    }
    int highest_supported_version_by_neighbor = highest + ((supported_versions_length - 1) * 8);
    // negate to indicate that we don't actually support it
    return -highest_supported_version_by_neighbor;
  }

  return highest_supported_version;
}
