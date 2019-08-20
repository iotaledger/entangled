cc_library(
    name = "storage",
    hdrs = [
        "connection.h",
        "defs.h",
        "storage.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":pack",
        "//ciri/consensus/snapshot:state_delta",
        "//common:errors",
        "//common/model:bundle",
        "//common/trinary:trit_array",
        "//utils:hash_maps",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_queue",
        "//utils/containers/hash:hash81_queue",
    ],
)

cc_library(
    name = "pack",
    srcs = ["pack.c"],
    hdrs = ["pack.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
    ],
)

genrule(
    name = "tangle_db",
    srcs = ["//common/storage/sql:tangle-schema"],
    outs = ["tangle.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "spent_addresses_db",
    srcs = ["//common/storage/sql:spent-addresses-schema"],
    outs = ["spent-addresses.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
    visibility = ["//visibility:public"],
)
