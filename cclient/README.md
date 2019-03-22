# IOTA CClient Library  

IOTA client library implementation in C.  

## Running test cases

```
bazel test //cclient/...
```

## Building library  

```
bazel build //cclient/api:libcclient.so
```

Output  

```
# shared libraries
./bazel-bin/cclient/api/libcclient.so
./bazel-bin/cclient/api/libapi_core.so
./bazel-bin/cclient/api/libapi_extended.so
# static libraries
./bazel-bin/cclient/api/libapi_core.a
./bazel-bin/cclient/api/libapi_extended.a
```

## Using CClient library  

CClient library is a subproject in the entangled repository, for using this library you can:  

* Link CClient shared or static libraries to your application.
* Create a bazel project and add CClient to the dependence.
* Port CClient to your build system.

Below show you how to create a bazel project.  

First thing is to create **WORKSPACE** and **BUILD** files in your project folder.

The **WORKSPACE** file  

```
git_repository(
    name = "entangled",
    commit = "b76278278c8481ee8c4fd52f16d64594bc0a48ed",
    remote = "https://github.com/iotaledger/entangled.git",
)

# external library build rules
git_repository(
    name = "rules_iota",
    commit = "39eeaf4fad15929b0fcc88b8fc473925a6bd9060",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

load("@rules_iota//:defs.bzl", "iota_deps")

iota_deps()
```

The **BUILD** file  

```
package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "app",
    copts = ["-DLOGGER_ENABLE"],
    srcs = ["cclient_app.c", "cclient_app.h",],
    deps = ["@entangled//cclient:api",],
    visibility = ["//visibility:public"],
)

```

Second include `cclient/api/core/core_api.h` and `cclient/api/extended/extended_api.h` in the source code.  

This is the example project: [iota_cclient_example](https://github.com/oopsmonk/iota_cclient_example)  

## API List  
C Client API consists of two API sets:  
* **Core APIs** are basic APIs in [API reference](https://docs.iota.org/docs/iri/0.1/references/api-reference).  
* **Extended APIs** are commonly used API functions for applications.

**Core APIs**  

* iota_client_get_node_info()
* iota_client_get_neighbors()
* iota_client_add_neighbors()
* iota_client_remove_neighbors()
* iota_client_get_tips()
* iota_client_find_transactions()
* iota_client_get_trytes()
* iota_client_get_inclusion_states()
* iota_client_get_balances()
* iota_client_get_transactions_to_approve()
* iota_client_attach_to_tangle()
* iota_client_interrupt_attaching_to_tangle()
* iota_client_broadcast_transactions()
* iota_client_store_transactions()
* iota_client_check_consistency()

**Extended APIs**  

* iota_client_broadcast_bundle()
* iota_client_find_transaction_objects()
* iota_client_get_account_data()
* iota_client_get_bundle()
* iota_client_get_inputs()
* iota_client_get_latest_inclusion()
* iota_client_get_new_address()
* iota_client_get_transaction_objects()
* iota_client_is_promotable()
* iota_client_prepare_transfers()
* iota_client_promote_transaction()
* iota_client_replay_bundle()
* iota_client_send_transfer()
* iota_client_send_trytes()
* iota_client_store_and_broadcast()
* iota_client_traverse_bundle()

## API Documentation  
*Work in progress.*  
