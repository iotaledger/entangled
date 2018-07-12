package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster",
    srcs = ["broadcaster.c"],
    hdrs = ["broadcaster.h"],
    deps = [
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)

cc_library(
    name = "processor",
    srcs = ["processor.c"],
    hdrs = ["processor.h"],
    deps = [
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)

cc_library(
    name = "responder",
    srcs = ["responder.c"],
    hdrs = ["responder.h"],
    deps = [
        "//common/network/queues:concurrent_queue_hash_request",
    ],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.c"],
    hdrs = ["receiver.h"],
)

cc_library(
    name = "requester",
    srcs = ["requester.c"],
    hdrs = ["requester.h"],
    deps = [
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)
