#[[
Copyright (c) 2018 IOTA Stiftung
https://github.com/iotaledger/entangled

Refer to the LICENSE file for licensing information
]]

# external libraries
include(../cmake/cjson.cmake)
include(../cmake/http_parser.cmake)
include(../cmake/unity.cmake)

#==============cclient source=====================
set(API_CORE_DIR "api/core")
set(API_CORE_SRC
  ${API_CORE_DIR}/add_neighbors.c
  ${API_CORE_DIR}/attach_to_tangle.c
  ${API_CORE_DIR}/broadcast_transactions.c
  ${API_CORE_DIR}/check_consistency.c
  ${API_CORE_DIR}/core_init.c
  ${API_CORE_DIR}/find_transactions.c
  ${API_CORE_DIR}/get_balances.c
  ${API_CORE_DIR}/get_inclusion_states.c
  ${API_CORE_DIR}/get_neighbors.c
  ${API_CORE_DIR}/get_node_info.c
  ${API_CORE_DIR}/get_tips.c
  ${API_CORE_DIR}/get_transactions_to_approve.c
  ${API_CORE_DIR}/get_trytes.c
  ${API_CORE_DIR}/logger.c
  ${API_CORE_DIR}/remove_neighbors.c
  ${API_CORE_DIR}/store_transactions.c
  ${API_CORE_DIR}/were_addresses_spent_from.c
)

set(API_EXTENDED_DIR "api/extended")
set(API_EXTENDED_SRC
  ${API_EXTENDED_DIR}/broadcast_bundle.c
  ${API_EXTENDED_DIR}/extended_init.c
  ${API_EXTENDED_DIR}/find_transaction_objects.c
  ${API_EXTENDED_DIR}/get_account_data.c
  ${API_EXTENDED_DIR}/get_bundle.c
  ${API_EXTENDED_DIR}/get_inputs.c
  ${API_EXTENDED_DIR}/get_latest_inclusion.c
  ${API_EXTENDED_DIR}/get_new_address.c
  ${API_EXTENDED_DIR}/get_transaction_objects.c
  ${API_EXTENDED_DIR}/is_promotable.c
  ${API_EXTENDED_DIR}/logger.c
  ${API_EXTENDED_DIR}/prepare_transfers.c
  ${API_EXTENDED_DIR}/promote_transaction.c
  ${API_EXTENDED_DIR}/replay_bundle.c
  ${API_EXTENDED_DIR}/send_transfer.c
  ${API_EXTENDED_DIR}/send_trytes.c
  ${API_EXTENDED_DIR}/store_and_broadcast.c
  ${API_EXTENDED_DIR}/traverse_bundle.c
)

set(HTTP_SRC "http/http.c")

set(REQ_DIR "request")
set(REQ_SRC
  ${REQ_DIR}/add_neighbors.c
  ${REQ_DIR}/attach_to_tangle.c
  ${REQ_DIR}/broadcast_transactions.c
  ${REQ_DIR}/check_consistency.c
  ${REQ_DIR}/find_transactions.c
  ${REQ_DIR}/get_balances.c
  ${REQ_DIR}/get_inclusion_states.c
  ${REQ_DIR}/get_transactions_to_approve.c
  ${REQ_DIR}/get_trytes.c
  ${REQ_DIR}/remove_neighbors.c
  ${REQ_DIR}/store_transactions.c
  ${REQ_DIR}/were_addresses_spent_from.c
)

set(RES_DIR "response")
set(RES_SRC
  ${RES_DIR}/add_neighbors.c
  ${RES_DIR}/attach_to_tangle.c
  ${RES_DIR}/check_consistency.c
  ${RES_DIR}/error.c
  ${RES_DIR}/find_transactions.c
  ${RES_DIR}/get_balances.c
  ${RES_DIR}/get_inclusion_states.c
  ${RES_DIR}/get_missing_transactions.c
  ${RES_DIR}/get_neighbors.c
  ${RES_DIR}/get_node_info.c
  ${RES_DIR}/get_tips.c
  ${RES_DIR}/get_transactions_to_approve.c
  ${RES_DIR}/get_trytes.c
  ${RES_DIR}/remove_neighbors.c
  ${RES_DIR}/were_addresses_spent_from.c
)

set(JSON_SERIALIZER_DIR "serialization/json")
set(JSON_SERIALIZER_SRC
  ${JSON_SERIALIZER_DIR}/add_neighbors.c
  ${JSON_SERIALIZER_DIR}/attach_to_tangle.c
  ${JSON_SERIALIZER_DIR}/broadcast_transactions.c
  ${JSON_SERIALIZER_DIR}/check_consistency.c
  ${JSON_SERIALIZER_DIR}/error.c
  ${JSON_SERIALIZER_DIR}/find_transactions.c
  ${JSON_SERIALIZER_DIR}/get_balances.c
  ${JSON_SERIALIZER_DIR}/get_inclusion_states.c
  ${JSON_SERIALIZER_DIR}/get_missing_transactions.c
  ${JSON_SERIALIZER_DIR}/get_neighbors.c
  ${JSON_SERIALIZER_DIR}/get_node_info.c
  ${JSON_SERIALIZER_DIR}/get_tips.c
  ${JSON_SERIALIZER_DIR}/get_transactions_to_approve.c
  ${JSON_SERIALIZER_DIR}/get_trytes.c
  ${JSON_SERIALIZER_DIR}/helpers.c
  ${JSON_SERIALIZER_DIR}/json_serializer.c
  ${JSON_SERIALIZER_DIR}/logger.c
  ${JSON_SERIALIZER_DIR}/remove_neighbors.c
  ${JSON_SERIALIZER_DIR}/store_transactions.c
  ${JSON_SERIALIZER_DIR}/were_addresses_spent_from.c
)

#==============end of cclient source=====================

# libcclient 
add_library(cclient 
  service.c
  ${API_CORE_SRC}
  ${API_EXTENDED_SRC}
  ${HTTP_SRC}
  ${REQ_SRC}
  ${RES_SRC}
  ${JSON_SERIALIZER_SRC}
)

add_library(entangled::cclient ALIAS cclient)

add_dependencies(cclient
  entangled::common
  cjson
  http_parser
)

target_link_libraries(cclient PUBLIC
  ${ENTANGLED_COMMON}
)

if(${CCLIENT_TEST})
  # ==============API test cases================
  function(add_cclient_test test_src test_name)
    add_executable(${test_name} "api/tests/${test_src}" "api/tests/cclient_service_init.c")
    if(${RPI_CROSS_COMPILATION})
      set_property(TARGET ${test_name} PROPERTY C_STANDARD 99)
    endif()
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR} ${EXTERNAL_INCLUDE_DIR})
    add_dependencies(${test_name} entangled::cclient unity)
    target_link_libraries(${test_name} PRIVATE
      ${ENTANGLED_CCLIENT}
      ${ENTANGLED_COMMON}
      ${EXTERNAL_LINK_LIBS}
      ${EXT_LIB_UNITY}
      Threads::Threads
    )
    add_test(${test_name} ${test_name})
  endfunction(add_cclient_test)

  add_cclient_test("test_add_neighbors.c" "test_add_neighbors")
  add_cclient_test("test_attach_to_tangle.c" "test_attach_to_tangle")
  add_cclient_test("test_broadcast_transactions.c" "test_broadcast_transactions")
  add_cclient_test("test_check_consistency.c" "test_check_consistency")
  add_cclient_test("test_find_transactions.c" "test_find_transactions")
  add_cclient_test("test_get_balances.c" "test_get_balances")
  add_cclient_test("test_get_inclusion_states.c" "test_get_inclusion_states")
  add_cclient_test("test_get_neighbors.c" "test_get_neighbors")
  add_cclient_test("test_get_node_info.c" "test_get_node_info")
  add_cclient_test("test_get_tips.c" "test_get_tips")
  add_cclient_test("test_get_transactions_to_approve.c" "test_get_transactions_to_approve")
  add_cclient_test("test_get_trytes.c" "test_get_trytes")
  add_cclient_test("test_remove_neighbors.c" "test_remove_neighbors")
  add_cclient_test("test_store_transactions.c" "test_store_transactions")
  add_cclient_test("test_were_addresses_spent_from.c" "test_were_addresses_spent_from")
  # ==============API test cases================
endif()

if(${RPI_CROSS_COMPILATION})
  set_property(TARGET cclient PROPERTY C_STANDARD 99)
endif()