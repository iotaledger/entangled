/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_FILES_H__
#define __UTILS_FILES_H__

#include <stdbool.h>

#include "common/errors.h"

#define FILE_PATH_SIZE 128

#ifdef _WIN32
#define IOTA_UTILS_FILE_SEPARATOR "\\"
#else
#define IOTA_UTILS_FILE_SEPARATOR "/"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Copies a file to a given path
 *
 * @param to The path of the destination file
 * @param from The path of the source file
 *
 * @return error code
 */
retcode_t iota_utils_copy_file(char const *const to, char const *const from);

/**
 * Removes an existing file
 *
 * @param file_path The path of the file
 *
 * @return error code
 */
retcode_t iota_utils_remove_file(char const *const file_path);

/**
 * Overwrites an existing file or creates a new one with provided content
 *
 * @param file_path The path of the file
 * @param content The content to write
 *
 * @return error code
 */
retcode_t iota_utils_overwrite_file(char const *const file_path, char const *const content);

/**
 * Reads a file content into buffer
 *
 * @param file_path The file to read
 * @param buffer The buffer to read into
 *
 * @return error code
 */
retcode_t iota_utils_read_file_into_buffer(char const *const file_path, char **const buffer);

/**
 * Checks if a file exist
 *
 * @param file_path The path of the file
 *
 * @return True if exists
 */
bool iota_utils_file_exist(char const *const file_path);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_FILES_H__
