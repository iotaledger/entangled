CREATE TABLE IF NOT EXISTS iota_transaction (
  signature_or_message BLOB NOT NULL,
  address BLOB NOT NULL,
  value INTEGER NOT NULL,
  obsolete_tag BLOB,
  timestamp INTEGER NOT NULL,
  current_index SMALLINT NOT NULL,
  last_index SMALLINT NOT NULL,
  bundle BLOB NOT NULL,
  trunk BLOB NOT NULL,
  branch BLOB NOT NULL,
  tag BLOB NOT NULL,
  attachment_timestamp INTEGER NOT NULL,
  attachment_timestamp_lower INTEGER NOT NULL,
  attachment_timestamp_upper INTEGER NOT NULL,
  nonce BLOB NOT NULL,
  hash BLOB NOT NULL PRIMARY KEY,
  snapshot_index INTEGER NOT NULL DEFAULT 0,
  solid SMALLINT NOT NULL DEFAULT 0,
  validity SMALLINT NOT NULL DEFAULT 0,
  arrival_timestamp INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS address_index ON iota_transaction(address);
CREATE INDEX IF NOT EXISTS bundle_index ON iota_transaction(bundle);
CREATE INDEX IF NOT EXISTS trunk_index ON iota_transaction(trunk);
CREATE INDEX IF NOT EXISTS branch_index ON iota_transaction(branch);
CREATE INDEX IF NOT EXISTS tag_index ON iota_transaction(tag);
CREATE INDEX IF NOT EXISTS transaction_hash_index ON iota_transaction(hash);
CREATE INDEX IF NOT EXISTS arrival_time_index ON iota_transaction(arrival_timestamp);

CREATE TABLE IF NOT EXISTS iota_milestone (
  id INTEGER NOT NULL PRIMARY KEY,
  hash BLOB NOT NULL UNIQUE,
  delta BLOB
);

CREATE INDEX IF NOT EXISTS milestone_hash_index ON iota_milestone(hash);
