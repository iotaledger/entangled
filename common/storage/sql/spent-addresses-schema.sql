CREATE TABLE IF NOT EXISTS iota_spent_address (
  hash BLOB NOT NULL PRIMARY KEY ON CONFLICT IGNORE
);

CREATE INDEX IF NOT EXISTS spent_address_hash_index ON iota_spent_address(hash);
