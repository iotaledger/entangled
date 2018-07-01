package(default_visibility = ["//visibility:public"])

cc_library(
    name = "concurrent_queue_transaction",
    hdrs = [
        "concurrent_queue.h.inc",
        "concurrent_queue_transaction.h",
    ],
    deps = [
        "//common/model:transaction",
    ],
)

cc_library(
    name = "concurrent_queue_hash_request",
    hdrs = [
        "concurrent_queue.h.inc",
        "concurrent_queue_hash_request.h",
    ],
    deps = [
        "//common/model:neighbor",
    ],
)

cc_library(
    name = "concurrent_queue_trytes",
    hdrs = [
        "concurrent_queue.h.inc",
        "concurrent_queue_trytes.h",
    ],
    deps = [
        "//common/trinary:tryte",
    ],
)
