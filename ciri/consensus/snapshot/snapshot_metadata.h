/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_SNAPSHOT_METADATA_H__
#define __CONSENSUS_SNAPSHOT_SNAPSHOT_METADATA_H__

#include <stdbool.h>
#include <stdint.h>

#include "ciri/consensus/conf.h"
#include "utils/containers/hash/hash_uint64_t_map.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snapshot_metadata_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  uint64_t index;
  uint64_t timestamp;
  hash_to_uint64_t_map_t solid_entry_points;
  // TODO - seen milestones
} snapshot_metadata_t;

/**
 * Initializes a snapshot metadata
 *
 * @param snapshot_metadata The snapshot metadata
 * @param hash The hash of the transaction the snapshot is based upon (milestone)
 * @param index The index of the transaction the snapshot is based upon (milestone)
 * @param timestamp The timestamp of the transaction the snapshot is based upon (milestone)
 * @param solid_entry_points A map containing solid entry points, (hash to snapshot_index)
 *
 * @return a status code
 */
retcode_t iota_snapshot_metadata_init(snapshot_metadata_t *const snapshot_metadata, flex_trit_t const *const hash,
                                      uint64_t index, uint64_t timestamp, hash_to_uint64_t_map_t solid_entry_points);

/**
 * Resets a snapshot metadata
 *
 * @param snapshot_metadata The snapshot metadata
 * @return void
 */
void iota_snapshot_metadata_reset(snapshot_metadata_t *const snapshot_metadata);

/**
 * Destroys a snapshot metadata
 *
 * @param snapshot_metadata The snapshot metadata
 *
 * @return a status code
 */
retcode_t iota_snapshot_metadata_destroy(snapshot_metadata_t *const snapshot_metadata);

/**
 * Returns the size for serialization of a snapshot_metadata_t struct
 *
 * @param snapshot_metadata The snapshot metadata
 *
 * @return a status code
 */
size_t iota_snapshot_metadata_serialized_str_size(snapshot_metadata_t const *const snapshot_metadata);

/**
 * Serializes metadata into string buffer
 *
 * @param snapshot_metadata The snapshot metadata
 * @param str The string buffer tp serialize to
 *
 * @return a status code
 */
retcode_t iota_snapshot_metadata_serialize_str(snapshot_metadata_t const *const snapshot_metadata, char *const str);

/**
 * Deserializes metadata from string buffer
 *
 * @param str The string buffer tp serialize to
 * @param snapshot_metadata The snapshot metadata
 *
 * @return a status code
 */
retcode_t iota_snapshot_metadata_deserialize_str(char const *const str, snapshot_metadata_t *const snapshot_metadata);

/**
 * Reads a snapshot  metadata from file
 *
 * @param snapshot_metadata The snapshot metadata
 * @param snapshot_file The file with serialized metadata
 *
 * @return a status code
 */
retcode_t iota_snapshot_metadata_read_from_file(snapshot_metadata_t *const snapshot_metadata,
                                                char const *const metadata_file);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOT_METADATA_H__
