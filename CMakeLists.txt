#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

cmake_minimum_required(VERSION 3.5)

cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

if(${CMAKE_VERSION} VERSION_LESS "3.9.6")
  project(entangled)
else()
  project(entangled DESCRIPTION "IOTA Entangled Monorepo")
endif()

enable_language(C)
enable_language(CXX)
enable_testing()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# default CLI options
option(CCLIENT "Enable CClient library" ON)
option(CCLIENT_TEST "Enable CClient library test cases" ON)

# PCurl configuration flags
option(PCURL_SBOX_UNWIND_2 "Unwind 2 rounds of PCurl sbox" ON)
option(PCURL_SBOX_UNWIND_4 "Unwind 4 rounds of PCurl sbox" OFF)
option(PCURL_SBOX_UNWIND_8 "Unwind 8 rounds of PCurl sbox" OFF)
option(PCURL_STATE_DOUBLE "Use PCurl double state; PCurl short state optimization is enabled by default" OFF)

include(ExternalProject)

# we compile and install entangled all dependencies in a sandbox instead of system dirs.
# change the default without overriding a user-provided value
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "..." FORCE)
  message(STATUS "set default install path: ${CMAKE_INSTALL_PREFIX}")
endif()

include_directories(
  "${CMAKE_INSTALL_PREFIX}/include"
  "${PROJECT_SOURCE_DIR}"
)

# libs in the sandbox
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

set(EXTERNAL_BUILD_DIR "${PROJECT_BINARY_DIR}/external")
set(EXTERNAL_DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/download")
set(EXTERNAL_INSTALL_DIR "${PROJECT_BINARY_DIR}/install")
set(EXTERNAL_INSTALL_BIN_DIR "${PROJECT_BINARY_DIR}/install/bin")
set(EXTERNAL_INCLUDE_DIR "${EXTERNAL_INSTALL_DIR}/include")
set(EXTERNAL_LINK_LIBS "")

# external libs
include(cmake/embear_logger.cmake)
include(cmake/keccak.cmake)
include(cmake/mbedtls.cmake)
include(cmake/unity.cmake)
include(cmake/uthash.cmake)

# subdirs
if(${CCLIENT})
add_subdirectory(cclient)
endif()

if(${MAM})
  add_subdirectory(mam)
endif()

set(COMMON_TRINARY_DIR "common/trinary")
set(COMMON_CRYPTO_DIR "common/crypto")
set(COMMON_HELPERS_DIR "common/helpers")
set(COMMON_MODEL_DIR "common/model")
set(COMMON_SRC
  # error code
  "common/errors.c"
  # trynary
  "${COMMON_TRINARY_DIR}/add.c"
  "${COMMON_TRINARY_DIR}/flex_trit.c"
  "${COMMON_TRINARY_DIR}/ptrit.c"
  "${COMMON_TRINARY_DIR}/ptrit_incr.c"
  "${COMMON_TRINARY_DIR}/trit_byte.c"
  "${COMMON_TRINARY_DIR}/trit_long.c"
  "${COMMON_TRINARY_DIR}/trit_tryte.c"
  "${COMMON_TRINARY_DIR}/tryte_ascii.c"
  "${COMMON_TRINARY_DIR}/tryte_long.c"
  # curl
  "${COMMON_CRYPTO_DIR}/curl-p/const.c"
  "${COMMON_CRYPTO_DIR}/curl-p/curl_p.c"
  "${COMMON_CRYPTO_DIR}/curl-p/digest.c"
  "${COMMON_CRYPTO_DIR}/curl-p/hashcash.c"
  "${COMMON_CRYPTO_DIR}/curl-p/pearl_diver.c"
  "${COMMON_CRYPTO_DIR}/curl-p/ptrit.c"
  # kerl
  "${COMMON_CRYPTO_DIR}/kerl/bigint.c"
  "${COMMON_CRYPTO_DIR}/kerl/converter.c"
  "${COMMON_CRYPTO_DIR}/kerl/kerl.c"
  "${COMMON_CRYPTO_DIR}/kerl/hash.c"
  # sign
  "${COMMON_CRYPTO_DIR}/iss/v1/iss_curl.c"
  "${COMMON_CRYPTO_DIR}/iss/v1/iss_kerl.c"
  "${COMMON_CRYPTO_DIR}/iss/normalize.c"
  # helpers
  "${COMMON_HELPERS_DIR}/checksum.c"
  "${COMMON_HELPERS_DIR}/digest.c"
  "${COMMON_HELPERS_DIR}/sign.c"
  "${COMMON_HELPERS_DIR}/pow.c"
  # models
  "${COMMON_MODEL_DIR}/bundle.c"
  "${COMMON_MODEL_DIR}/transaction.c"
  "${COMMON_MODEL_DIR}/transfer.c"
)

set(HASH_CONTAINERS_DIR "utils/containers/hash")
# genrate hash containers
add_custom_command(
  OUTPUT
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash27_queue.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash81_queue.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash243_queue.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash6561_queue.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash8019_queue.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash27_stack.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash81_stack.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash243_stack.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash6561_stack.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash8019_stack.c"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash27_queue.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash81_queue.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash243_queue.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash6561_queue.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash8019_queue.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash27_stack.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash81_stack.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash243_stack.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash6561_stack.h"
  "${PROJECT_SOURCE_DIR}/${HASH_CONTAINERS_DIR}/hash8019_stack.h"
  COMMAND "./cmake/gen_hash_container.sh"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

set(UTILS_SRC
  "utils/time.c"
  "utils/logger_helper.c"
  "utils/handles/socket.c"
  "utils/char_buffer.c"
  "utils/memset_safe.c"
  "utils/system.c"
  # hash container
  "${HASH_CONTAINERS_DIR}/hash_array.c"
  "${HASH_CONTAINERS_DIR}/hash27_queue.c"
  "${HASH_CONTAINERS_DIR}/hash81_queue.c"
  "${HASH_CONTAINERS_DIR}/hash243_queue.c"
  "${HASH_CONTAINERS_DIR}/hash6561_queue.c"
  "${HASH_CONTAINERS_DIR}/hash8019_queue.c"
  "${HASH_CONTAINERS_DIR}/hash27_stack.c"
  "${HASH_CONTAINERS_DIR}/hash81_stack.c"
  "${HASH_CONTAINERS_DIR}/hash243_stack.c"
  "${HASH_CONTAINERS_DIR}/hash6561_stack.c"
  "${HASH_CONTAINERS_DIR}/hash8019_stack.c"
)

add_library(common
  "${COMMON_SRC}"
  "${UTILS_SRC}"
)

if(${PCURL_SBOX_UNWIND_8})
  target_compile_definitions(common PRIVATE "-DPCURL_SBOX_UNWIND_8")
elseif(${PCURL_SBOX_UNWIND_4})
  target_compile_definitions(common PRIVATE "-DPCURL_SBOX_UNWIND_4")
elseif(${PCURL_SBOX_UNWIND_2})
  target_compile_definitions(common PRIVATE "-DPCURL_SBOX_UNWIND_2")
else()
  # Unwind 2 loops by default
  target_compile_definitions(common PRIVATE "-DPCURL_SBOX_UNWIND_2")
endif()

if(${PCURL_STATE_DOUBLE})
  target_compile_definitions(common PRIVATE "-DPCURL_STATE_DOUBLE")
else()
  target_compile_definitions(common PRIVATE "-DPCURL_STATE_SHORT")
endif()

add_library(entangled::common ALIAS common)
target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR})
add_dependencies(common
  ext_uthash
  ext_keccak
  ext_embear_logger
  ext_mbedtls
)

list(APPEND EXTERNAL_LINK_LIBS "${EXTERNAL_LINK_LIBS}" Threads::Threads)
target_link_libraries(common PUBLIC "${EXTERNAL_LINK_LIBS}")

# install
install(TARGETS common DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/common/" DESTINATION "${CMAKE_INSTALL_PREFIX}/include/entangled/common" FILES_MATCHING PATTERN "*.h")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/utils/" DESTINATION "${CMAKE_INSTALL_PREFIX}/include/entangled/utils" FILES_MATCHING PATTERN "*.h")

# remove build files
add_custom_target(clean_build
  COMMAND make clean
  COMMAND rm -rf ${EXTERNAL_BUILD_DIR}
    ${PROJECT_BINARY_DIR}/install
    Makefile
    cclient
    CMake*
    CTest*
    Testing
    cmake_install.cmake
    build_*
    install_manifest.txt
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# remove download cashes
add_custom_target(clean_download
  COMMAND rm -rf ${EXTERNAL_DOWNLOAD_DIR}
)

# empty build dir
add_custom_target(distclean
  COMMAND make clean clean_build clean_download
)

if(${RPI_CROSS_COMPILATION})
  set_property(TARGET common PROPERTY C_STANDARD 99)
endif()
