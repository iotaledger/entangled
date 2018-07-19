package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster",
    srcs = ["broadcaster.c"],
    hdrs = ["broadcaster.h"],
    deps = [
        "//common:thread_handle",
        "//common/network:logger",
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)

cc_library(
    name = "processor",
    srcs = ["processor.c"],
    hdrs = ["processor.h"],
    deps = [
        "//common:thread_handle",
        "//common/network:logger",
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)

cc_library(
    name = "responder",
    srcs = ["responder.c"],
    hdrs = ["responder.h"],
    deps = [
        "//common:thread_handle",
        "//common/network:logger",
        "//common/network/queues:concurrent_queue_hash_request",
    ],
)

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = [
        "//common:thread_handle",
    ],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.c"],
    deps = [
        ":receiver_shared",
        "//common/network/services:receiver",
    ],
)

cc_library(
    name = "requester",
    srcs = ["requester.c"],
    hdrs = ["requester.h"],
    deps = ["//common/network/queues:concurrent_queue_trit_array"],
)
