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
        "//ciri/node:node_shared",
        "//ciri/node/pipeline:transaction_requester",
        "//common/network:uri",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "router",
    srcs = ["router.c"],
    hdrs = ["router.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":neighbor_shared",
        "//ciri/node:conf",
        "//utils:logger_helper",
        "//utils/handles:rw_lock",
    ],
)

cc_library(
    name = "tcp_server_shared",
    hdrs = ["tcp_server.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//utils/handles:thread",
        "@libuv",
    ],
)

cc_library(
    name = "tcp_server",
    srcs = ["tcp_server.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":tcp_server_shared",
        "//ciri/node:node_shared",
    ],
)
