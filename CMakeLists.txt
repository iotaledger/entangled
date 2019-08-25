#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

cmake_minimum_required(VERSION 3.5)

cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

project(entangled DESCRIPTION "IOTA Entangled Monorepo")

enable_language(C)
enable_language(CXX)
enable_testing()

# default CLI options
option(CCLIENT "Enable CClient library" ON)
option(CCLIENT_TEST "Enable CClient library test cases" ON)

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

set(EXTERNAL_BUILD_DIR "${PROJECT_BINARY_DIR}/external")
set(EXTERNAL_DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/download")
set(EXTERNAL_INSTALL_DIR "${PROJECT_BINARY_DIR}/install")
set(EXTERNAL_INCLUDE_DIR "${EXTERNAL_INSTALL_DIR}/include")
set(EXTERNAL_LINK_LIBS "")
set(ENTANGLED_COMMON "${PROJECT_BINARY_DIR}/libcommon.a")
set(ENTANGLED_CCLIENT "${PROJECT_BINARY_DIR}/cclient/libcclient.a")

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


set(COMMON_SRC 
  # error code 
  "common/errors.c"
  # trynary
  "common/trinary/add.c"
  "common/trinary/flex_trit.c"
  "common/trinary/ptrit_incr.c"
  "common/trinary/trit_array.c"
  "common/trinary/trit_byte.c"
  "common/trinary/trit_long.c"
  "common/trinary/trit_ptrit.c"
  "common/trinary/trit_tryte.c"
  "common/trinary/tryte_ascii.c"
  "common/trinary/tryte_long.c"
  # curl
  "common/crypto/curl-p/const.c"
  "common/crypto/curl-p/curl_p.c"
  "common/crypto/curl-p/digest.c"
  #"common/crypto/curl-p/hashcash.c"
  # kerl
  "common/crypto/kerl/bigint.c"
  "common/crypto/kerl/converter.c"
  "common/crypto/kerl/kerl.c"
  "common/crypto/kerl/hash.c"
  # sign
  "common/crypto/iss/v1/iss_curl.c"
  "common/crypto/iss/v1/iss_kerl.c"
  "common/crypto/iss/normalize.c"
  # helpers
  "common/helpers/checksum.c"
  "common/helpers/digest.c"
  "common/helpers/sign.c"
  #"common/helpers/pow.c"
  # models
  "common/model/bundle.c"
  "common/model/transaction.c"
  "common/model/transfer.c"
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
  COMMAND "./cmake/gen_hash_container.sh"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

set(UTILS_SRC 
  "utils/time.c"
  "utils/logger_helper.c"
  "utils/handles/socket.c"
  "utils/char_buffer.c"
  "utils/memset_safe.c"
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

add_library(entangled::common ALIAS common)
target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR})
add_dependencies(common 
  uthash
  keccak
  embear_logger
  mbedtls
)

target_link_libraries(common PUBLIC "${EXTERNAL_LINK_LIBS}")

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
