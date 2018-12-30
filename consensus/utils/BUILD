cc_library(
    name = "tangle_traversals",
    srcs = ["tangle_traversals.c"],
    hdrs = ["tangle_traversals.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/model:transaction",
        "//common/storage:pack",
        "//consensus/tangle",
        "//utils/containers/hash:hash243_set",
        "//utils/containers/hash:hash243_stack",
    ],
)

cc_library(
    name = "tangle_simulator",
    srcs = ["tangle_simulator.c"],
    hdrs = ["tangle_simulator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//consensus/cw_rating_calculator",
    ],
)
