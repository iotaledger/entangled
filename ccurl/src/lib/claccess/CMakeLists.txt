cmake_minimum_required(VERSION 3.5) enable_language(C) find_package(Threads)
    find_library(OPENCL_LIBRARIES OpenCL) project(clcontext)

        set(CMAKE_INSTALL_PREFIX "/usr")

            file(GLOB SRC*.c) set(SOURCES ${SRC})

                set(LIBRARY_NAME clcontext)

                    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall")

                        SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} /
                            bin) SET(CTEST_BUILD_TARGET "DEVELOPMENT")

                            add_library(${LIBRARY_NAME} SHARED ${SRC})

                                set_target_properties(
                                    ${LIBRARY_NAME} PROPERTIES
                                            LIBRARY_OUTPUT_DIRECTORY ${
                                                PROJECT_BINARY_DIR} /
                                        lib LINK_FLAGS -
                                    lpthread)
#target_include_directories(curl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR })

                                    install(TARGETS ${
                                        LIBRARY_NAME} LIBRARY DESTINATION lib
                                                ARCHIVE DESTINATION lib RUNTIME
                                                    DESTINATION bin)

#Install library headers
                                        file(GLOB HEADERS*.h) install(FILES ${
                                            HEADERS} DESTINATION include)

#enable_testing()
#add_subdirectory(test)
