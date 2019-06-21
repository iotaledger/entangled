package(default_visibility = ["//visibility:public"])

load(":conf.bzl", "GOSSIP_MAINNET_VARIABLES")
load(":conf.bzl", "GOSSIP_TESTNET_VARIABLES")
load("//utils/containers:map_generator.bzl", "map_generate")

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    defines = select({
        "//ciri:mainnet": GOSSIP_MAINNET_VARIABLES,
        "//ciri:testnet": GOSSIP_TESTNET_VARIABLES,
        "//conditions:default": GOSSIP_MAINNET_VARIABLES,
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
        "//gossip/components:transaction_requester",
        "//gossip/services:tcp_sender",
        "//gossip/services:udp_sender",
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
    parent_directory = "gossip",
    value_type = "flex_trit_t",
)

cc_library(
    name = "node_shared",
    hdrs = ["node.h"],
    deps = [
        ":neighbor_shared",
        ":recent_seen_bytes_cache",
        ":tips_cache",
        "//gossip/components:broadcaster_shared",
        "//gossip/components:processor_shared",
        "//gossip/components:receiver_shared",
        "//gossip/components:responder_shared",
        "//gossip/components:tips_requester_shared",
        "//gossip/components:tips_solidifier_shared",
        "//gossip/components:transaction_requester_shared",
        "//gossip/components:transaction_requester_worker_shared",
    ],
)

cc_library(
    name = "node",
    srcs = ["node.c"],
    deps = [
        ":node_shared",
        "//ciri:core_shared",
        "//gossip/components:broadcaster",
        "//gossip/components:processor",
        "//gossip/components:receiver",
        "//gossip/components:responder",
        "//gossip/components:tips_requester",
        "//gossip/components:tips_solidifier",
        "//gossip/components:transaction_requester",
        "//gossip/components:transaction_requester_worker",
    ],
)
