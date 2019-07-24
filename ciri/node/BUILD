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
        ":uint64_t_to_flex_trit_t_map",
        "//ciri/node/protocol:gossip",
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils/handles:lock",
        "@xxhash",
    ],
)

map_generate(
    additional_deps = ["//common/trinary:flex_trit"],
    additional_include_path = "common/trinary/flex_trit.h",
    key_type = "uint64_t",
    parent_directory = "ciri/node",
    value_type = "flex_trit_t",
)

cc_library(
    name = "node_shared",
    hdrs = ["node.h"],
    deps = [
        ":recent_seen_bytes_cache",
        ":tips_cache",
        "//ciri/node/network:router_shared",
        "//ciri/node/pipeline:broadcaster_shared",
        "//ciri/node/pipeline:hasher_shared",
        "//ciri/node/pipeline:processor_shared",
        "//ciri/node/pipeline:responder_shared",
        "//ciri/node/pipeline:tips_requester_shared",
        "//ciri/node/pipeline:transaction_requester_shared",
    ],
)

cc_library(
    name = "node",
    srcs = ["node.c"],
    deps = [
        ":node_shared",
        "//ciri:core_shared",
        "//ciri/node/network:router",
        "//ciri/node/pipeline:broadcaster",
        "//ciri/node/pipeline:hasher",
        "//ciri/node/pipeline:processor",
        "//ciri/node/pipeline:responder",
        "//ciri/node/pipeline:tips_requester",
        "//ciri/node/pipeline:transaction_requester",
    ],
)
