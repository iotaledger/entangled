CREATE TABLE IF NOT EXISTS iota_transaction (
  signature_or_message BLOB NOT NULL,
  address BLOB NOT NULL,
  value INTEGER DEFAULT 0 NOT NULL,
  obsolete_tag BLOB,
  timestamp TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  current_index SMALLINT DEFAULT 0 NOT NULL,
  last_index SMALLINT DEFAULT 0 NOT NULL,
  bundle BLOB NOT NULL,
  trunk BLOB NOT NULL,
  branch BLOB NOT NULL,
  tag BLOB NOT NULL,
  attachment_timestamp TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  attachment_timestamp_lower TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  attachment_timestamp_upper TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  nonce BLOB NOT NULL
);
