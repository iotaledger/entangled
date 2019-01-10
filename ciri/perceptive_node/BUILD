package(default_visibility = ["//visibility:public"])


load("//utils/containers:map_generator.bzl", "map_generate")


map_generate(
    additional_deps = ["//gossip:neighbor"],
    additional_include_path = "gossip/neighbor.h",
    key_type = "neighbor_t",
    parent_directory = "ciri/perceptive_node",
    value_type = "uint32_t",
)


cc_library(
    name = "perceptive_node",
    srcs = ["perceptive_node.c"],
    hdrs = ["perceptive_node.h"],
    deps = [
        ":conf",
        "//consensus",
        "//consensus/utils:tangle_simulator",
        ":neighbor_t_to_uint32_t_map",
        "//utils/containers/arrays:double_array",
        "//utils/containers/hash:hash_array",
        "//utils/containers/hash:hash_hash_pair_t_map",
    ],
)

cc_library(
    name = "conf",
    srcs = [
        "conf.c",
        "conf.h",
    ],
    hdrs = ["conf.h"],
    deps = [
        "//common:errors",
    ],
)
