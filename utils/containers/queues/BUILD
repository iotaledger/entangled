package(default_visibility = ["//visibility:public"])

cc_library(
    name = "concurrent_queue_packet",
    srcs = ["concurrent_queue_packet.c"],
    hdrs = ["concurrent_queue_packet.h"],
    deps = [
        ":concurrent_queue",
        "//common/network:iota_packet",
    ],
)

cc_library(
    name = "concurrent_queue_transaction_request",
    srcs = ["concurrent_queue_transaction_request.c"],
    hdrs = ["concurrent_queue_transaction_request.h"],
    deps = [
        ":concurrent_queue",
        "//common/network:transaction_request",
    ],
)

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
