/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_ERRORS_H_
#define COMMON_ERRORS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file  erros.h
 *
 * This interface provides type definitions, data types and functions to handle
 * exceptions.
 *
 * The error code is designed to fit into a single, 16 bit integer
 * The higher bits representing the module, the lower bits represent
 * the code within the module. The two high bits of the module
 * are interpreted as the severity of the error code.
 *
 * The stack itself uses modules in the range 1..127.
 * Thus an application may use values 128..255 for own purposes.
 *
 * M is the module, S the severity and C the actual error code
 *  1
 *  5      8        0
 * +--------+--------+
 * |MMMMMMMM|SSCCCCCC|
 * +--------+--------+
 *
 * This yields 255 modules and 255 codes per module.
 * In the naive construction like it is done below,
 * the severity is treated separately and thus only 63 codes per
 * module are actually used.
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
#define RC_MODULE_STORAGE_SQLITE3 (0x02 << RC_SHIFT_MODULE)

/* error code module specific */
#define RC_ERRORCODE_MASK 0x003F

/* Macros for Severity of error code */
#define RC_GET_SEVERITY(x) ((x)&RC_SEVERITY_MASK)

/* Macros for module of error code */
#define RC_GET_MODULE(x) ((x)&RC_MODULE_MASK)

/* Macros for Error code module specific */
#define RC_GET_ERRORCODE(x) ((x)&RC_ERRORCODE_MASK)


/** Return Codes */
enum retcode_t {
  /** Success (equal to zero) */
  RC_OK = 0,
  RC_SQLITE3_FAILED_OPEN_DB =
      0x01 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_FATAL,
  RC_SQLITE3_FAILED_INSERT_DB =
      0x02 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_MAJOR,
  RC_SQLITE3_FAILED_CREATE_INDEX_DB =
      0x03 | RC_MODULE_STORAGE_SQLITE3 | RC_SEVERITY_MAJOR,
};

#ifdef __cplusplus
}
#endif

#endif  // COMMON_ERRORS_H_
