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

C Client library is a subproject under the entangled, for using this library you should create a new bazel project and add this library to the deptendance.  
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
