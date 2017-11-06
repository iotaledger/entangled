cmake_minimum_required (VERSION 3.2)
enable_language (C)
#find_package (OpenCL)
if (WIN32 AND NOT MINGW)
else()
    find_package (Threads)
endif()
include(InstallRequiredSystemLibraries)
project (ccurl)

set(CMAKE_INSTALL_PREFIX "/usr")
set (LIBRARY_NAME ccurl)
#set (OPENCL_LIBRARIES ${CMAKE_SOURCE_DIR}/deps/libopencl-stub/build/lib/libOpenCL.a)
set (CL_LIBRARY_NAME OpenCL)
set (OPENCL_LIBRARIES ${CL_LIBRARY_NAME})
add_subdirectory (deps/libopencl-stub)

#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -g -O0")
if (WIN32 AND NOT MINGW)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ")
else()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -O3")
    #set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -O0")
endif()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin) 
SET (CTEST_BUILD_TARGET "RELEASE")


#### OPENCL FILES

SET(RESOURCE_COMPILER xxd)
FILE(GLOB COMPILED_RESOURCES "src/lib/cl/*")
FOREACH(INPUT_FILE ${COMPILED_RESOURCES})
	get_filename_component(INPUT_NAME ${INPUT_FILE} NAME)
	get_filename_component(INPUT_PATH ${INPUT_FILE} DIRECTORY)
	SET(OUTPUT_FILE ${INPUT_PATH}/../${INPUT_NAME}.h)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${OUTPUT_FILE}
        COMMAND ${RESOURCE_COMPILER} -i ${INPUT_NAME} ${OUTPUT_FILE}
		WORKING_DIRECTORY ${INPUT_PATH}
		)
	LIST(APPEND COMPILED_RESOURCES ${OUTPUT_FILE})
ENDFOREACH()

#### LIBCCURL
file(GLOB SRC src/lib/*.c src/lib/claccess/*.c src/lib/util/*.c)
set (SOURCES ${SRC})

include_directories (${CMAKE_SOURCE_DIR}/deps/libopencl-stub/include)
add_library (${LIBRARY_NAME} SHARED ${SOURCES} ${COMPILED_RESOURCES})
add_library (${LIBRARY_NAME}static STATIC ${SOURCES} ${COMPILED_RESOURCES})
if (WIN32 AND NOT MINGW)
    target_link_libraries(${LIBRARY_NAME} ${OPENCL_LIBRARIES} )
    target_link_libraries(${LIBRARY_NAME}static ${OPENCL_LIBRARIES})
else()
    #target_link_libraries(${LIBRARY_NAME} dl ${OPENCL_LIBRARIES})
    target_link_libraries(${LIBRARY_NAME} dl ${OPENCL_LIBRARIES})
    target_link_libraries(${LIBRARY_NAME}static dl ${OPENCL_LIBRARIES})
endif()


if (WIN32 AND NOT MINGW)
    set_target_properties (${LIBRARY_NAME} PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY lib
        )
    set_target_properties(${LIBRARY_NAME}static PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY lib
        OUTPUT_NAME ${LIBRARY_NAME}
        )
else()
    set_target_properties (${LIBRARY_NAME} PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY lib
        LINK_FLAGS -lpthread
        )
    set_target_properties(${LIBRARY_NAME}static PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY lib
        OUTPUT_NAME ${LIBRARY_NAME}
        LINK_FLAGS -lpthread
        )
endif()

install(TARGETS ${LIBRARY_NAME} ${LIBRARY_NAME}static
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin)

set(PACKAGE_NAME "CCurl")
set(PACKAGE_VENDOR "IOTA Foundation")
set(PACKAGE_DESCRIPTION "CCurl - IOTA POW Accelerator")
set(PACKAGE_VERSION "0.2.0")
configure_file ("ccurl.pc.in" "${PROJECT_BINARY_DIR}/ccurl.pc" @ONLY)
install (FILES "${PROJECT_BINARY_DIR}/ccurl.pc" DESTINATION lib/pkgconfig)

# Install library headers
FILE(GLOB HEADERS "src/lib/*.h")
install(FILES ${HEADERS} DESTINATION include/ccurl)
FILE(GLOB HEADERSUTIL "src/lib/util/*.h")
install(FILES ${HEADERSUTIL} DESTINATION include/ccurl/util)

### TESTING
#enable_testing ()
#add_subdirectory (test)
add_subdirectory (src/cli)

set(CPACK_PACKAGE_NAME "CCurl")
set(CPACK_PACKAGE_VENDOR "IOTA Foundation")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CCurl - IOTA POW Accelerator")
#set(CPACK_PACKAGE_VERSION "0.2.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CCurl")
set(CPACK_PACKAGE_CONTACT "Paul Handy (iota@pauldhandy.com)")
set(CPACK_NSIS_MODIFY_PATH "ON")
include(CPack)
