project (ccurl-cli)
if (WIN32 AND NOT MINGW)
    set(ws2_LIBRARY_PATH Ws2_32)
elseif (MINGW)
    set(ws2_LIBRARY_PATH ws2_32)
endif()
set (CLI_SOURCES ccurl.c)
set (CCD_SOURCES ccurld.c)
set (DIGEST_SOURCE ccurl-digest.c)
set (LONG_SOURCE trytes-to-long.c)
set (TRITS_SOURCE trytes-to-trits.c)

set (CLI_NAME ccurl-cli)
set (CCD_NAME ccurld)
set (DIGEST_NAME ccurl-digest)
set (LONG_NAME trytes-to-long)
set (TRITS_NAME trytes-to-trits)

#add_executable (${CLI_NAME} ${CLI_SOURCES} ../lib/PearlDiver.c ../lib/pearcldiver.c ../lib/exports.c)
#add_executable (${CLI_NAME} ${CLI_SOURCES} ${SOURCES})
add_executable (${CLI_NAME} ${CLI_SOURCES})
add_executable (${CCD_NAME} ${CCD_SOURCES})
add_executable (${DIGEST_NAME} ${DIGEST_SOURCE})
add_executable (${LONG_NAME} ${LONG_SOURCE})
add_executable (${TRITS_NAME} ${TRITS_SOURCE})
if (WIN32 AND NOT MINGW)
set_target_properties (${CLI_NAME} PROPERTIES 
	BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	)
set_target_properties (${DIGEST_NAME} PROPERTIES 
	BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	)
else()
    set_target_properties (${TRITS_NAME} PROPERTIES 
	LINK_FLAGS -Wl,-lpthread
	BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	)
    set_target_properties (${LONG_NAME} PROPERTIES 
	LINK_FLAGS -Wl,-lpthread
	BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	)
set_target_properties (${DIGEST_NAME} PROPERTIES 
	LINK_FLAGS -Wl,-lpthread
	BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	)
set_target_properties (${CLI_NAME} PROPERTIES 
	LINK_FLAGS -Wl,-lpthread
	BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	)
set_target_properties (${CCD_NAME} PROPERTIES 
	LINK_FLAGS -Wl,-lpthread
	BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
	)
endif()
#target_link_libraries(${CLI_NAME} ${OPENCL_LIBRARIES} )
target_link_libraries(${CLI_NAME} ${ws2_LIBRARY_PATH} ${LIBRARY_NAME}static)
target_link_libraries(${CCD_NAME} ${ws2_LIBRARY_PATH} ${LIBRARY_NAME}static)
target_link_libraries(${DIGEST_NAME} ${ws2_LIBRARY_PATH} ${LIBRARY_NAME}static)
target_link_libraries(${LONG_NAME} ${ws2_LIBRARY_PATH} ${LIBRARY_NAME}static)
target_link_libraries(${TRITS_NAME} ${ws2_LIBRARY_PATH} ${LIBRARY_NAME}static)

install(TARGETS ${TRITS_NAME} 
	RUNTIME DESTINATION bin)
install(TARGETS ${LONG_NAME} 
	RUNTIME DESTINATION bin)
install(TARGETS ${DIGEST_NAME} 
	RUNTIME DESTINATION bin)
install(TARGETS ${CLI_NAME} 
	RUNTIME DESTINATION bin)
install(TARGETS ${CCD_NAME} 
	RUNTIME DESTINATION bin)
