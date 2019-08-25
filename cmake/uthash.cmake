#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

if (NOT __UTHASH_INCLUDED)
  set(__UTHASH_INCLUDED TRUE)

  ExternalProject_Add(
    uthash 
    PREFIX ${EXTERNAL_BUILD_DIR}/uthash
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_DIR}
    DOWNLOAD_NAME uthash_v2.1.0.tar.gz
    URL https://github.com/troydhanson/uthash/archive/v2.1.0.tar.gz
    URL_HASH SHA256=152ccd8e64d0f495377232e3964d06c7ec8bb8c3fbd3217f8a5702614f9a669e
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
                    <SOURCE_DIR>/src ${CMAKE_INSTALL_PREFIX}/include
  )

endif()