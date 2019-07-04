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
