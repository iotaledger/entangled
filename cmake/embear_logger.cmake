#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

if (NOT __EMBEAR_LOGGER_INCLUDED)
  set(__EMBEAR_LOGGER_INCLUDED TRUE)

  ExternalProject_Add(
    ext_embear_logger
    PREFIX ${EXTERNAL_BUILD_DIR}/embear_logger
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_DIR}
    DOWNLOAD_NAME embear_logger_v4.0.x.tar.gz
    URL https://github.com/embear/logger/archive/v4.0.x.tar.gz
    URL_HASH SHA256=af36b44bbc4aea3618962f68e24bc1efcc2c53d490b8b4a56463ea7afc910d67
    # copy header 
    UPDATE_COMMAND ${CMAKE_COMMAND} -E copy_directory
                    <SOURCE_DIR>/include ${CMAKE_INSTALL_PREFIX}/include
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
      -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    # for debug
    # LOG_DOWNLOAD 1
    # LOG_CONFIGURE 1
    # LOG_INSTALL 1
  )

  list(APPEND EXTERNAL_LINK_LIBS logger)

endif()