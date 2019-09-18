#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

if (NOT __UNITY_INCLUDED)
  set(__UNITY_INCLUDED TRUE)

  ExternalProject_Add(
    unity_download
    PREFIX ${EXTERNAL_BUILD_DIR}/unity
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_DIR}
    DOWNLOAD_NAME unity_v2.4.3.tar.gz
    URL https://github.com/ThrowTheSwitch/Unity/archive/v2.4.3.tar.gz
    URL_HASH SHA256=a8c5e384f511a03c603bbecc9edc24d2cb4a916998d51a29cf2e3a2896920d03
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
    # for debug
    # LOG_DOWNLOAD 1
  )

  set(unity_cmake_dir ${EXTERNAL_BUILD_DIR}/unity/src/ext_unity)
  set(unity_src_dir ../unity_download)
  set(unity_install_include ${CMAKE_INSTALL_PREFIX}/include/unity)
  set(unity_install_lib ${CMAKE_INSTALL_PREFIX}/lib)

  file(WRITE ${unity_cmake_dir}/CMakeLists.txt
    "cmake_minimum_required(VERSION 3.5)\n"
    "project(unity C)\n"
    "set(my_src ${unity_src_dir}/src/unity.c)\n"
    "add_library(unity STATIC \${my_src})\n"
    "target_include_directories(unity\n"
    "PUBLIC ${unity_src_dir}/src)\n"
    "install(TARGETS unity DESTINATION ${unity_install_lib})\n"
    "install(DIRECTORY ${unity_src_dir}/src/ DESTINATION ${unity_install_include} FILES_MATCHING PATTERN \"*.h\")\n"
  )

  ExternalProject_Add(
    ext_unity
    PREFIX ${EXTERNAL_BUILD_DIR}/unity
    DOWNLOAD_COMMAND ""
    BUILD_IN_SOURCE TRUE
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
      -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    # for debug
    # LOG_CONFIGURE 1
    # LOG_INSTALL 1
  )
  add_dependencies(ext_unity unity_download)
  set(EXT_LIB_UNITY unity)

endif()