package(default_visibility = ["//visibility:public"])

load(":conf.bzl", "NODE_MAINNET_VARIABLES")
load(":conf.bzl", "NODE_TESTNET_VARIABLES")
load("//utils/containers:map_generator.bzl", "map_generate")

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    defines = select({
        "//ciri:mainnet": NODE_MAINNET_VARIABLES,
        "//ciri:testnet": NODE_TESTNET_VARIABLES,
        "//conditions:default": NODE_MAINNET_VARIABLES,
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils:files",
    ],
)

cc_library(
    name = "iota_packet",
    srcs = ["iota_packet.c"],
    hdrs = ["iota_packet.h"],
    deps = [
        ":conf",
        "//common:errors",
        "//common/model:transaction",
        "//common/network:endpoint",
        "//common/trinary:bytes",
        "//common/trinary:flex_trit",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "neighbor_shared",
    hdrs = ["neighbor.h"],
    deps = [
        ":iota_packet",
        "//common:errors",
    ],
)

cc_library(
    name = "neighbor",
    srcs = ["neighbor.c"],
    deps = [
        ":neighbor_shared",
        ":node_shared",
        "//common/network:uri",
        "//node/pipeline:transaction_requester",
        "//node/services:tcp_sender",
        "//node/services:udp_sender",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "transaction_request",
    srcs = ["transaction_request.c"],
    hdrs = ["transaction_request.h"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "tips_cache",
    srcs = ["tips_cache.c"],
    hdrs = ["tips_cache.h"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils/containers/hash:hash243_set",
        "//utils/containers/hash:hash243_stack",
        "//utils/handles:rw_lock",
    ],
)

cc_library(
    name = "recent_seen_bytes_cache",
    srcs = ["recent_seen_bytes_cache.c"],
    hdrs = ["recent_seen_bytes_cache.h"],
    deps = [
        ":iota_packet",
        ":uint64_t_to_flex_trit_t_map",
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils/handles:rw_lock",
        "@xxhash",
    ],
)

map_generate(
    additional_deps = ["//common/trinary:flex_trit"],
    additional_include_path = "common/trinary/flex_trit.h",
    key_type = "uint64_t",
    parent_directory = "node",
    value_type = "flex_trit_t",
)

cc_library(
    name = "node_shared",
    hdrs = ["node.h"],
    deps = [
        ":neighbor_shared",
        ":recent_seen_bytes_cache",
        ":tips_cache",
        "//node/pipeline:broadcaster_shared",
        "//node/pipeline:processor_shared",
        "//node/pipeline:receiver_shared",
        "//node/pipeline:responder_shared",
        "//node/pipeline:tips_requester_shared",
        "//node/pipeline:tips_solidifier_shared",
        "//node/pipeline:transaction_requester_shared",
        "//node/pipeline:transaction_requester_worker_shared",
    ],
)

cc_library(
    name = "node",
    srcs = ["node.c"],
    deps = [
        ":node_shared",
        "//ciri:core_shared",
        "//node/pipeline:broadcaster",
        "//node/pipeline:processor",
        "//node/pipeline:receiver",
        "//node/pipeline:responder",
        "//node/pipeline:tips_requester",
        "//node/pipeline:tips_solidifier",
        "//node/pipeline:transaction_requester",
        "//node/pipeline:transaction_requester_worker",
    ],
)
