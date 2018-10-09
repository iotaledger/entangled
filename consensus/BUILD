cc_library(
    name = "consensus",
    srcs = ["consensus.c"],
    hdrs = ["consensus.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
    ],
)

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "model",
    hdrs = ["model.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
    ],
)
