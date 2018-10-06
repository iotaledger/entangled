# IOTA C Client Library  

IOTA client library implementation in C.  

## Building library  

```
bazel build //cclient:api
```

Output  

```
./bazel-bin/cclient/libapi.a
./bazel-bin/cclient/libapi.so
```

## Using CClient library  

C Client library is a subproject in the entangled repository, for using this library you should create a new bazel project and add this library to the dependence.  
First thing is to create **WORKSPACE** and **BUILD** files in your project folder, second include `iota_client_core_api.h` in the source code.  

The **WORKSPACE** file  

```
# local_repository(
#     name = "entangled",
#     path = "/path/to/entangled",
# )

git_repository(
    name = "entangled",
    commit = "b57bd1fce59c8d9b6fcc2bcfbb960c0ef14ee261",
    remote = "https://github.com/iotaledger/entangled.git",
)

git_repository(
    name = "rules_iota",
    commit = "18179db1ce0be893643de847a4e673f371d89ec8",
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
    includes = ["external/entangled/cclient/"],
    deps = ["@entangled//cclient:api",],
    visibility = ["//visibility:public"],
)

```

C Client application example [iota_cclient_example](https://github.com/oopsmonk/iota_cclient_example)  

## API List  
C Client API consisted of two API sets:  
* **Core APIs** are basic APIs provide by [IRI](https://github.com/iotaledger/iri).  
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

**Extended AIPs**  
*Work in progress.*  

## API Documentation  
*Work in progress.*  
