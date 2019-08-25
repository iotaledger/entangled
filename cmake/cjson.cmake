#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

if (NOT __CJSON_INCLUDED)
  set(__CJSON_INCLUDED TRUE)

  ExternalProject_Add(
    cjson
    PREFIX ${EXTERNAL_BUILD_DIR}/cjson
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_DIR}
    DOWNLOAD_NAME cjson_v1.7.12.tar.gz
    URL https://github.com/DaveGamble/cJSON/archive/v1.7.12.tar.gz
    URL_HASH SHA256=760687665ab41a5cff9c40b1053c19572bcdaadef1194e5cba1b5e6f824686e7
    CMAKE_ARGS
    -DENABLE_CJSON_TEST:STRING=Off
    -DENABLE_CJSON_UTILS:STRING=Off
    -DBUILD_SHARED_LIBS:STRING=Off
    -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    # for debug
    # LOG_DOWNLOAD 1
    # LOG_CONFIGURE 1
    # LOG_INSTALL 1
  )

  include_directories("${CMAKE_INSTALL_PREFIX}/include/cjson")
  list(APPEND EXTERNAL_LINK_LIBS "${CMAKE_INSTALL_PREFIX}/lib/libcjson.a")

endif()