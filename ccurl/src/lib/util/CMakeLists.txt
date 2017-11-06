cmake_minimum_required (VERSION 3.5)
enable_language (C)
project (iotaconverter)

set(CMAKE_INSTALL_PREFIX "/usr")


set (LIBRARY_NAME iotaconverter)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -g -O0")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin) 
SET (CTEST_BUILD_TARGET "RELEASE")

set (SOURCES converter.c)

#add_library(${LIBRARY_NAME } SHARED ${SOURCES })
add_library (${LIBRARY_NAME} SHARED ${SOURCES} )
#target_link_libraries(${LIBRARY_NAME } ${OPENCL_LIBRARIES })

set_target_properties (${LIBRARY_NAME} PROPERTIES 
	LIBRARY_OUTPUT_DIRECTORY lib
#LINK_FLAGS - lpthread
	)

install(TARGETS ${LIBRARY_NAME} 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin)

#Install library headers
file(GLOB HEADERS src/lib/*.h)
install(FILES ${HEADERS} DESTINATION include)
