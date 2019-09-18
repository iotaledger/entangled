#[[
Copyright (c) 2019 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

if (NOT __RPI_TOOLCHAIN_INCLUDED)

  # toolchain: https://github.com/raspberrypi/tools
  # export RPI_TOOLS_PATH="path-to-raspberry-pi-tools"
  # cmake .. -DCMAKE_TOOLCHAIN_FILE=${PWD}/../cmake/rpi_toolchain.cmake 
  set(__RPI_TOOLCHAIN_INCLUDED TRUE)

  # check env settings: RPI_TOOLS_PATH, RPI_SYSROOT_PATH
  set(_rpi_tools_errors)
  string(COMPARE EQUAL "$ENV{RPI_TOOLS_PATH}" "" _is_empty)
  if(_is_empty)
    set(_rpi_tools_errors "RPI_TOOLS_PATH environment variable is not set")
  endif()
  string(COMPARE NOTEQUAL "${_rpi_tools_errors}" "" _has_errors)
  if(_has_errors)
    message(${_rpi_tools_errors})
    message(FATAL_ERROR "Raspberry Pi cross toolchain configure error!...")
  endif()

  # Define host system
  set(CMAKE_SYSTEM_NAME Linux)
  set(CMAKE_SYSTEM_VERSION 1)

  set(CMAKE_C_COMPILER   $ENV{RPI_TOOLS_PATH}/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
  set(CMAKE_CXX_COMPILER $ENV{RPI_TOOLS_PATH}/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
  set(CMAKE_FIND_ROOT_PATH $ENV{RPI_TOOLS_PATH}/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/arm-linux-gnueabihf/sysroot/)
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  # Search for libraries and headers in the target directories only
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

  set(RPI_CROSS_COMPILATION TRUE)

endif()