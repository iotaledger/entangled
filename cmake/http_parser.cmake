#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

if (NOT __HTTP_PARSER_INCLUDED)
  set(__HTTP_PARSER_INCLUDED TRUE)

  ExternalProject_Add(
    http_parser_download
    PREFIX ${EXTERNAL_BUILD_DIR}/http_parser
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_DIR}
    DOWNLOAD_NAME http_parser_v2.9.2.tar.gz
    URL https://github.com/nodejs/http-parser/archive/v2.9.2.tar.gz
    URL_HASH SHA256=5199500e352584852c95c13423edc5f0cb329297c81dd69c3c8f52a75496da08
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
  )

  set(http_parser_cmake_dir ${EXTERNAL_BUILD_DIR}/http_parser/src/http_parser)
  set(http_parser_src_dir ../http_parser_download)
  set(http_parser_install_include ${CMAKE_INSTALL_PREFIX}/include)
  set(http_parser_install_lib ${CMAKE_INSTALL_PREFIX}/lib)

  file(WRITE ${http_parser_cmake_dir}/CMakeLists.txt
    "cmake_minimum_required(VERSION 3.5)\n"
    "project(http_parser C)\n"
    "set(my_src ${http_parser_src_dir}/http_parser.c)\n"
    "add_library(http_parser STATIC \${my_src})\n"
    "target_include_directories(http_parser\n"
    "PUBLIC ${http_parser_src_dir})\n"
    "install(TARGETS http_parser DESTINATION ${http_parser_install_lib})\n"
    "install(DIRECTORY ${http_parser_src_dir}/ DESTINATION ${http_parser_install_include} FILES_MATCHING PATTERN \"*.h\")\n"
  )

  ExternalProject_Add(
    http_parser
    PREFIX ${EXTERNAL_BUILD_DIR}/http_parser
    DOWNLOAD_COMMAND ""
    BUILD_IN_SOURCE TRUE
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
      -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    # for debug
    # LOG_DOWNLOAD 1
    # LOG_CONFIGURE 1
    # LOG_INSTALL 1
  )
  add_dependencies(http_parser http_parser_download) 

  list(APPEND EXTERNAL_LINK_LIBS "${CMAKE_INSTALL_PREFIX}/lib/libhttp_parser.a")

endif()