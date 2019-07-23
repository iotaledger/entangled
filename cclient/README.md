# IOTA CClient Library  

IOTA client library implementation in C.  

## Running test cases

```
$ bazel test //cclient/...
```

## Using CClient library in a Bazel project.  

CClient library is a component in the entangled repository, in order to use it in a Bazle project
* create a new Bazle project  
* import CClient library into the project  

First is to create **WORKSPACE** and **BUILD** files for the Bazel project, for more details, please visit [Bazel Concepts and Terminology](https://docs.bazel.build/versions/master/build-ref.html).  

The **WORKSPACE** file  

```
git_repository(
    name = "entangled",
    commit = "ENTANGLED_COMMIT_HASH",
    remote = "https://github.com/iotaledger/entangled.git",
)

# external library build rules
git_repository(
    name = "rules_iota",
    commit = "RULES_IOTA_COMMIT_HASH",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

load("@rules_iota//:defs.bzl", "iota_deps")

iota_deps()
```

The **BUILD** file  

```shell
package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "cclient_app",
    copts = ["-DLOGGER_ENABLE"],
    srcs = ["cclient_app.c", "cclient_app.h",],
    deps = ["@entangled//cclient:api",],
    visibility = ["//visibility:public"],
)

```

You can find an example project in: [iota_cclient_example](https://github.com/oopsmonk/iota_cclient_example)  



## Building shared library  

```
$ bazel build //cclient/api:libcclient.so
```

Output  

```
$ entangled/bazel-bin/cclient/api/libcclient.so
```

## API List  
C Client API consists of two API sets:  
* **Core APIs** are basic APIs in [API reference](https://docs.iota.org/docs/iri/0.1/references/api-reference).  
* **Extended APIs** are commonly used API functions for applications.

**Core APIs**  

* iota_client_add_neighbors()
* iota_client_attach_to_tangle()
* iota_client_broadcast_transactions()
* iota_client_check_consistency()
* iota_client_find_transactions()
* iota_client_get_balances()
* iota_client_get_inclusion_states()
* iota_client_get_neighbors()
* iota_client_get_node_info()
* iota_client_get_tips()
* iota_client_get_transactions_to_approve()
* iota_client_get_trytes()
* iota_client_remove_neighbors()
* iota_client_store_transactions()
* iota_client_were_addresses_spent_from()

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

The documentation can genetrated by [Doxygen](http://www.doxygen.nl/).  

```
$ cd entangled
$ doxygen ./Doxyfile
```

After building the document, you are able to open the document in `entangled/out/html/index.html` via Chrome or Firefox.  
