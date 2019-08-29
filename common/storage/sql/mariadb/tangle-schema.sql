CREATE TABLE IF NOT EXISTS iota_transaction (
  signature_or_message BLOB NOT NULL,
  address BLOB NOT NULL,
  value BIGINT NOT NULL,
  obsolete_tag BLOB,
  timestamp BIGINT NOT NULL,
  current_index SMALLINT NOT NULL,
  last_index SMALLINT NOT NULL,
  bundle BLOB NOT NULL,
  trunk BLOB NOT NULL,
  branch BLOB NOT NULL,
  tag BLOB NOT NULL,
  attachment_timestamp BIGINT NOT NULL,
  attachment_timestamp_lower BIGINT NOT NULL,
  attachment_timestamp_upper BIGINT NOT NULL,
  nonce BLOB NOT NULL,
  hash BLOB NOT NULL,
  snapshot_index INTEGER NOT NULL DEFAULT 0,
  solid SMALLINT NOT NULL DEFAULT 0,
  validity SMALLINT NOT NULL DEFAULT 0,
  arrival_timestamp BIGINT NOT NULL,
  PRIMARY KEY (hash(243))
);

CREATE INDEX IF NOT EXISTS address_index ON iota_transaction(address(243));
CREATE INDEX IF NOT EXISTS bundle_index ON iota_transaction(bundle(243));
CREATE INDEX IF NOT EXISTS trunk_index ON iota_transaction(trunk(243));
CREATE INDEX IF NOT EXISTS branch_index ON iota_transaction(branch(243));
CREATE INDEX IF NOT EXISTS tag_index ON iota_transaction(tag(81));
CREATE INDEX IF NOT EXISTS arrival_time_index ON iota_transaction(arrival_timestamp);
CREATE UNIQUE INDEX IF NOT EXISTS transaction_hash_index ON iota_transaction(hash(243));

CREATE TABLE IF NOT EXISTS iota_milestone (
  id INTEGER NOT NULL,
  hash BLOB NOT NULL,
  delta LONGBLOB,
  PRIMARY KEY (id)
);

CREATE UNIQUE INDEX IF NOT EXISTS milestone_hash_index ON iota_milestone(hash(243));
