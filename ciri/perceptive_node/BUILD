package(default_visibility = ["//visibility:public"])

cc_library(
    name = "perceptive_node",
    srcs = ["perceptive_node.c"],
    hdrs = ["perceptive_node.h"],
    deps = [
        ":conf",
        "//consensus",
        "//utils/containers/hash:hash_array",
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
