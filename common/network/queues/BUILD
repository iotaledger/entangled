package(default_visibility = ["//visibility:public"])

cc_library(
    name = "concurrent_queue",
    hdrs = ["concurrent_queue.h.inc"],
    deps = [
        "//common:cond_handle",
        "//common:lock_handle",
    ],
)

cc_library(
    name = "concurrent_queue_hash_request",
    hdrs = ["concurrent_queue_hash_request.h"],
    deps = [
        ":concurrent_queue",
        "//common/network:neighbor",
        "//common/trinary:trit_array",
    ],
)

cc_library(
    name = "concurrent_queue_trit_array",
    hdrs = ["concurrent_queue_trit_array.h"],
    deps = [
        ":concurrent_queue",
        "//common/trinary:trit_array",
    ],
)
