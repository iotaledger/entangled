package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster",
    hdrs = ["broadcaster.h"],
    deps = [
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)

cc_library(
    name = "processor",
    hdrs = ["processor.h"],
    deps = [
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)

cc_library(
    name = "responder",
    hdrs = ["responder.h"],
    deps = [
        "//common/network/queues:concurrent_queue_hash_request",
    ],
)

cc_library(
    name = "receiver",
    hdrs = ["receiver.h"],
)

cc_library(
    name = "requester",
    hdrs = ["requester.h"],
    deps = [
        "//common/network/queues:concurrent_queue_trit_array",
    ],
)
