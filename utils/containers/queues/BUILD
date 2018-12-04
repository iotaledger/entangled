package(default_visibility = ["//visibility:public"])

cc_library(
    name = "concurrent_queue_trit_array",
    srcs = ["concurrent_queue_trit_array.c"],
    hdrs = ["concurrent_queue_trit_array.h"],
    deps = [
        ":concurrent_queue",
        "//common/trinary:trit_array",
    ],
)

cc_library(
    name = "concurrent_queue",
    srcs = ["concurrent_queue.c.inc"],
    hdrs = ["concurrent_queue.h.inc"],
    deps = [
        "//utils/handles:cond",
        "//utils/handles:lock",
    ],
)
