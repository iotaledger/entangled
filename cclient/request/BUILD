cc_library(
    name = "requests",
    srcs = glob([
        "*.c",
    ]),
    hdrs = glob([
        "*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//utils/containers/hash:hash243_queue",
        "//utils/containers/hash:hash81_queue",
        "//utils/containers/hash:hash_array",
    ],
)
