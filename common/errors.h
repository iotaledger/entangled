/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_ERRORS_H_
#define COMMON_ERRORS_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file  errors.h
 *
 * The following error scheme takes advantage of a 16-bit integer
 * in order to fit in data about an error, its origin and severity
 *
 * bits devision:
 * 1 -> 6 - actual error code (63 errors)
 * 7 -> 8 - error's severity (4 categories)
 * 9 -> 16 - the module which originated the error (255 different modules)
 *
 * M is the module, S the severity and C the actual error code
 *
 *  16      9        1
 * *--------*--------*
 * |MMMMMMMM|SSCCCCCC|
 * *--------*--------*
 *
 */

/* error severity */
#define RC_SEVERITY_MASK 0x00C0
#define RC_SHIFT_SEVERITY 6

#define RC_SEVERITY_FATAL (0x3 << RC_SHIFT_SEVERITY)
#define RC_SEVERITY_MAJOR (0x2 << RC_SHIFT_SEVERITY)
#define RC_SEVERITY_MODERATE (0x1 << RC_SHIFT_SEVERITY)
#define RC_SEVERITY_MINOR (0x0 << RC_SHIFT_SEVERITY)

/* error location (module/component) */
#define RC_MODULE_MASK 0xFF00
#define RC_SHIFT_MODULE 8

#define RC_MODULE_STORAGE (0x01 << RC_SHIFT_MODULE)
#define RC_MODULE_STORAGE_SQL (0x02 << RC_SHIFT_MODULE)
#define RC_MODULE_STORAGE_SQLITE3 (0x03 << RC_SHIFT_MODULE)
#define RC_MODULE_CORE (0x04 << RC_SHIFT_MODULE)
#define RC_MODULE_NODE (0x05 << RC_SHIFT_MODULE)

/* error code module specific */
#define RC_ERRORCODE_MASK 0x003F

/* Macros for Severity of error code */
#define RC_GET_SEVERITY(x) ((x)&RC_SEVERITY_MASK)

/* Macros for module of error code */
#define RC_GET_MODULE(x) ((x)&RC_MODULE_MASK)

/* Macros for Error code module specific */
#define RC_GET_ERRORCODE(x) ((x)&RC_ERRORCODE_MASK)

#define RC_RESOLVE_FORMAT_STR "M=0x%02X, E=0x%02X, S=0x%X (0x%04X)"

/** Return Codes */
enum retcode_t {
  // Success (equal to zero)
  RC_OK = 0,

  // Storage SQLite3 Module
  RC_SQLITE3_FAILED_OPEN_DB =
      0x01 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_FATAL,
  RC_SQLITE3_FAILED_INSERT_DB =
      0x02 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_MAJOR,
  RC_SQLITE3_FAILED_CREATE_INDEX_DB =
      0x03 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_MAJOR,
  RC_SQLITE3_FAILED_WRITE_STATEMENT =
      0x04 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_MAJOR,
  RC_SQLITE3_FAILED_SELECT_DB =
      0x05 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_MAJOR,
  RC_SQLITE3_NO_PATH_FOR_DB_SPECIFIED =
      0x06 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_FATAL,
  RC_SQLITE3_FAILED_NOT_IMPLEMENTED =
      0x07 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_MAJOR,

  // Storage SQL Module
  RC_SQL_FAILED_WRITE_STATEMENT =
      0x01 | RC_MODULE_STORAGE_SQL | RC_SEVERITY_MAJOR,

  // Core Module
  RC_CORE_NULL_CORE = 0x01 | RC_MODULE_CORE | RC_SEVERITY_FATAL,
  RC_CORE_FAILED_DATABASE_INIT = 0x02 | RC_MODULE_CORE | RC_SEVERITY_FATAL,
  RC_CORE_FAILED_NODE_INIT = 0x03 | RC_MODULE_CORE | RC_SEVERITY_FATAL,
  RC_CORE_FAILED_NODE_START = 0x04 | RC_MODULE_CORE | RC_SEVERITY_FATAL,
  RC_CORE_FAILED_NODE_STOP = 0x05 | RC_MODULE_CORE | RC_SEVERITY_MODERATE,
  RC_CORE_FAILED_NODE_DESTROY = 0x06 | RC_MODULE_CORE | RC_SEVERITY_MODERATE,
  RC_CORE_FAILED_DATABASE_DESTROY =
      0x07 | RC_MODULE_CORE | RC_SEVERITY_MODERATE,

  // Node Module
  RC_NODE_NULL_NODE = 0x01 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_NULL_CORE = 0x02 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_NEIGHBORS_INIT = 0x03 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_BROADCASTER_INIT = 0x04 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_BROADCASTER_START = 0x05 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_BROADCASTER_STOP =
      0x06 | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_BROADCASTER_DESTROY =
      0x07 | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_PROCESSOR_INIT = 0x08 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_PROCESSOR_START = 0x09 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_PROCESSOR_STOP = 0x0A | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_PROCESSOR_DESTROY =
      0x0B | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_RECEIVER_INIT = 0x0C | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_RECEIVER_START = 0x0D | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_RECEIVER_STOP = 0x0E | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_RECEIVER_DESTROY =
      0x0F | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_REQUESTER_INIT = 0x10 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_REQUESTER_DESTROY =
      0x11 | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_RESPONDER_INIT = 0x12 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_RESPONDER_START = 0x13 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
  RC_NODE_FAILED_RESPONDER_STOP = 0x14 | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_FAILED_RESPONDER_DESTROY =
      0x15 | RC_MODULE_NODE | RC_SEVERITY_MODERATE,
  RC_NODE_STILL_RUNNING = 0x16 | RC_MODULE_NODE | RC_SEVERITY_FATAL,
};

typedef enum retcode_t retcode_t;

#ifdef __cplusplus
}
#endif

#endif  // COMMON_ERRORS_H_
