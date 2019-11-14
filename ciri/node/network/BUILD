cc_library(
    name = "endpoint",
    srcs = ["endpoint.c"],
    hdrs = ["endpoint.h"],
    visibility = ["//visibility:public"],
    deps = [":network"],
)

cc_library(
    name = "neighbor_shared",
    hdrs = ["neighbor.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/node/protocol:gossip",
        "//common:errors",
    ],
)

cc_library(
    name = "neighbor",
    srcs = ["neighbor.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":neighbor_shared",
        ":uri",
        "//ciri/node:node_shared",
        "//ciri/node/pipeline:transaction_requester",
        "//ciri/node/protocol",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "router_shared",
    hdrs = ["router.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":neighbor_shared",
        "//ciri/node:conf",
        "//ciri/node/protocol",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
        "@libuv",
    ],
)

cc_library(
    name = "router",
    srcs = ["router.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":neighbor",
        ":router_shared",
        "//ciri/node:node_shared",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "network",
    hdrs = ["network.h"],
)

cc_library(
    name = "uri",
    srcs = ["uri.c"],
    hdrs = ["uri.h"],
    deps = [":network"],
)
