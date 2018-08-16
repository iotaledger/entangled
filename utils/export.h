/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_EXPORT_H_
#define __UTILS_EXPORT_H_

#if defined(_MSC_VER)
#define IOTA_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define IOTA_EXPORT __attribute__((visibility("default")))
#else
#define IOTA_EXPORT
#warning Unknown dynamic link import / export semantics.
#endif

#endif  // __UTILS_EXPORT_H_
