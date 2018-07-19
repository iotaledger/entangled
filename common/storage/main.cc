/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "connection.h"

DEFINE_string(dbPath, "", "path to db file");
DEFINE_bool(indexAddress, true, "Should create an index for address");
DEFINE_bool(indexApprovee, true, "Should create an index for branch/trunk");
DEFINE_bool(indexBundle, true, "Should create an index for bundle");
DEFINE_bool(indexTag, true, "Should create an index for tag");

int main(int argc, char* argv[]) {

  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging("dummy storage");

  connection_config_t conf;
  conf.dbPath = FLAGS_dbPath.c_str();
  conf.indexAddress = FLAGS_indexAddress;
  conf.indexApprovee = FLAGS_indexApprovee;
  conf.indexBundle = FLAGS_indexBundle;
  conf.indexTag = FLAGS_indexTag;
  connection_t conn;
  init_connection(&conn, &conf);
  destroy_connection(&conn);
}