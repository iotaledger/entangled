CREATE TABLE IF NOT EXISTS iota_transaction (
  signature_or_message BLOB NOT NULL,
  address BLOB NOT NULL,
  value INTEGER NOT NULL,
  obsolete_tag BLOB,
  timestamp TIMESTAMP NOT NULL,
  current_index SMALLINT NOT NULL,
  last_index SMALLINT NOT NULL,
  bundle BLOB NOT NULL,
  trunk BLOB NOT NULL,
  branch BLOB NOT NULL,
  tag BLOB NOT NULL,
  attachment_timestamp TIMESTAMP NOT NULL,
  attachment_timestamp_lower TIMESTAMP NOT NULL,
  attachment_timestamp_upper TIMESTAMP NOT NULL,
  nonce BLOB NOT NULL,
  hash BLOB NOT NULL PRIMARY KEY,
  snapshot_index INTEGER NOT NULL,
  solid SMALLINT NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS iota_milestone (
  id INTEGER NOT NULL PRIMARY KEY,
  hash BLOB NOT NULL UNIQUE,
  delta BLOB
);
