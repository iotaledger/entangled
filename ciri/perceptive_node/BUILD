package(default_visibility = ["//visibility:public"])

cc_library(
    name = "perceptive_node",
    srcs = ["perceptive_node.c"],
    hdrs = ["perceptive_node.h"],
    deps = [
        ":conf",
        "//consensus",
        "//consensus/utils:tangle_simulator",
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
