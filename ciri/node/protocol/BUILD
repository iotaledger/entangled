load("//ciri/node:conf.bzl", "NODE_MAINNET_VARIABLES")
load("//ciri/node:conf.bzl", "NODE_TESTNET_VARIABLES")

cc_library(
    name = "gossip",
    srcs = ["gossip.c"],
    hdrs = ["gossip.h"],
    defines = select({
        "//ciri:mainnet": NODE_MAINNET_VARIABLES,
        "//ciri:testnet": NODE_TESTNET_VARIABLES,
        "//conditions:default": NODE_MAINNET_VARIABLES,
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/node/network:endpoint",
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:bytes",
        "//common/trinary:flex_trit",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "handshake",
    srcs = ["handshake.c"],
    hdrs = ["handshake.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/trinary:bytes",
        "//utils:macros",
        "//utils:time",
    ],
)

cc_library(
    name = "header",
    hdrs = ["header.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "protocol",
    hdrs = ["protocol.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":gossip",
        ":handshake",
        ":header",
    ],
)

cc_library(
    name = "transaction_request",
    srcs = ["transaction_request.c"],
    hdrs = ["transaction_request.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "@com_github_uthash//:uthash",
    ],
)
