enable_testing ()

FILE(GLOB THE_TESTS "test*.c")
FOREACH(TEST_TO_RUN ${THE_TESTS})
    get_filename_component(TEST_NAME ${TEST_TO_RUN} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" TEST_WITHOUT_EXT ${TEST_NAME})

    add_executable (${TEST_WITHOUT_EXT} ${TEST_TO_RUN} ./cunit_include.c ${SOURCES})

if (WIN32)
    set_target_properties (${TEST_WITHOUT_EXT} PROPERTIES LINK_FLAGS -Wl,-lbcunit)
else()
    set_target_properties (${TEST_WITHOUT_EXT} PROPERTIES LINK_FLAGS -Wl,-lbcunit,-lpthread)
endif()
    target_link_libraries(${TEST_WITHOUT_EXT} ${OPENCL_LIBRARIES} ${CLCONTEXT_LIBRARY})

    add_test (${TEST_WITHOUT_EXT} ${EXECUTABLE_OUTPUT_PATH}/${TEST_WITHOUT_EXT})

	LIST(APPEND COMPILED_RESOURCES ${OUTPUT_FILE})
ENDFOREACH()
