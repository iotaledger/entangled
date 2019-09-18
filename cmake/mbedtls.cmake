#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

if (NOT __MBEDTLS_INCLUDED)
  set(__MBEDTLS_INCLUDED TRUE)

  ExternalProject_Add(
    mbedtls_download
    PREFIX ${EXTERNAL_BUILD_DIR}/mbedtls
    SOURCE_DIR ${EXTERNAL_BUILD_DIR}/mbedtls/src/ext_mbedtls
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_DIR}
    DOWNLOAD_NAME mbedtls_v2.17.0.tar.gz
    URL https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.17.0.tar.gz
    URL_HASH SHA256=321a2c0220d9f75703e929c01dabba7b77da4cf2e39944897fc41d888bb1ef0d
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
    # for debug
    # LOG_DOWNLOAD 1
  )

  ExternalProject_Add(
    crypto_download
    PREFIX ${EXTERNAL_BUILD_DIR}/mbedtls
    SOURCE_DIR ${EXTERNAL_BUILD_DIR}/mbedtls/src/ext_mbedtls/crypto
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_DIR}
    DOWNLOAD_NAME crypto_a78c958b17d75ddf.tar.gz
    URL https://github.com/ARMmbed/mbed-crypto/archive/a78c958b17d75ddf63d8dd17255b6379dcbf259f.tar.gz
    URL_HASH SHA256=96538dde4fc400290b80651038f79079e910d7d29aece203a6378f885129f8cf
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
    # for debug
    # LOG_DOWNLOAD 1
  )

  ExternalProject_Add(
    ext_mbedtls
    PREFIX ${EXTERNAL_BUILD_DIR}/mbedtls
    DOWNLOAD_COMMAND ""
    CMAKE_ARGS
      -DENABLE_TESTING=Off 
      -DENABLE_PROGRAMS=Off
      -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
      -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    # for debug
    # LOG_CONFIGURE 1
    # LOG_INSTALL 1
  )

  add_dependencies(crypto_download mbedtls_download )
  add_dependencies(ext_mbedtls crypto_download)
  list(APPEND EXTERNAL_LINK_LIBS
    mbedtls
    mbedcrypto
    mbedx509
  )

endif()