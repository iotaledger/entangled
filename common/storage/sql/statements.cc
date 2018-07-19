/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include <string.h>
#include <string>

#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"

//TODO - should figure a better way without memcpy the whol statement
//       Maybe should use C all the way

retcode_t iota_transactions_insert_statement(const iota_transaction_t tx,
                                             char statement[]) {
  // FIXME(tsvi) - when iota_transaction PR for supporting trit_array is merged,
  // sizes should be different
  char sig_or_msg[2187 + 1];
  char address[81 + 1];
  char obsolete_tag[27];
  char bundle[81];
  char trunk[81];
  char branch[81];
  char tag[27];
  char nonce[27];

  // FIXME (tsvi) SIZES!
  memcpy(sig_or_msg, tx->signature_or_message, 2187);
  sig_or_msg[2187] = '\0';
  memcpy(address, tx->address, 81);
  address[81] = '\0';
  std::string value = std::to_string(tx->value);
  std::string timestamp = std::to_string(tx->timestamp);
  memcpy(obsolete_tag, tx->obsolete_tag, 27);
  obsolete_tag[27] = '\0';
  std::string current_index = std::to_string(tx->current_index);
  std::string last_index = std::to_string(tx->last_index);

  memcpy(bundle, tx->bundle, 81);
  bundle[81] = '\0';
  memcpy(trunk, tx->trunk, 81);
  trunk[81] = '\0';
  memcpy(branch, tx->branch, 81);
  branch[81] = '\0';
  memcpy(tag, tx->tag, 27);
  tag[27] = '\0';

  std::string attachment_timestamp = std::to_string(tx->attachment_timestamp);
  std::string attachment_timestamp_upper =
      std::to_string(tx->attachment_timestamp_upper);
  std::string attachment_timestamp_lower =
      std::to_string(tx->attachment_timestamp_lower);

  memcpy(nonce, tx->nonce, 27);
  nonce[27] = '\0';

  std::string statementStr =
      std::string("INSERT INTO ") + TRANSACTION_TABLE_NAME + "(" +
      COL_SIG_OR_MSG + "," + COL_ADDRESS + "," + COL_VALUE + "," +
      COL_OBSOLETE_TAG + "," + COL_TIMESTAMP + "," + COL_CURRENT_INDEX + "," +
      COL_LAST_INDEX + "," + COL_BUNDLE + "," + COL_TRUNK + "," + COL_BRANCH +
      "," + COL_TAG + "," + COL_ATTACHMENT_TIMESTAMP + "," +
      COL_ATTACHMENT_TIMESTAMP_UPPER + "," + COL_ATTACHMENT_TIMESTAMP_LOWER +
      "," + COL_NONCE + ") VALUES (" + sig_or_msg + "," + "'" + address + "'" +
      "," + value.c_str() + "," + "'" + obsolete_tag + "'" + "," + timestamp +
      "," + current_index.c_str() + "," + last_index.c_str() + "," + "'" +
      bundle + "'" + "," + "'" + trunk + "'" + "," + "'" + branch + "'" + "," +
      "'" + tag + "'" + "," + attachment_timestamp.c_str() + "," +
      attachment_timestamp_upper.c_str() + "," +
      attachment_timestamp_lower.c_str() + "," + "'" + nonce + "'" + ");";

  memcpy(statement, statementStr.c_str(), statementStr.size());
  statement[statementStr.size()] = '\0';
}

retcode_t iota_transactions_select_statement(const char* index_name,
                                             const trit_array_p key,
                                             char statement[]) {}

retcode_t iota_transactions_update_statement(const char* index_name,
                                             const trit_array_p key,
                                             const iota_transaction_t tx,
                                             char statement[]) {}