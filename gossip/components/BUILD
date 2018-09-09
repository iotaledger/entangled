package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster_shared",
    hdrs = ["broadcaster.h"],
    deps = ["//utils/handles:thread"],
)

cc_library(
    name = "broadcaster",
    srcs = ["broadcaster.c"],
    deps = [
        ":broadcaster_shared",
        "//ciri:node_shared",
        "//utils:logger_helper",
        "//utils/containers/lists:concurrent_list_neighbor",
        "//utils/containers/queues:concurrent_queue_packet",
    ],
)

cc_library(
    name = "processor_shared",
    hdrs = ["processor.h"],
    deps = ["//utils/handles:thread"],
)

cc_library(
    name = "processor",
    srcs = ["processor.c"],
    deps = [
        ":processor_shared",
        "//ciri:core_shared",
        "//utils/containers/lists:concurrent_list_neighbor",
        "//utils/containers/queues:concurrent_queue_packet",
    ],
)

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = ["//gossip/services:receiver_shared"],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.c"],
    deps = [
        ":receiver_shared",
        "//ciri:node_shared",
        "//gossip/services:receiver",
    ],
)

cc_library(
    name = "requester_shared",
    hdrs = ["requester.h"],
)

cc_library(
    name = "requester",
    srcs = ["requester.c"],
    deps = [
        ":requester_shared",
        "//ciri:core_shared",
        "//utils:logger_helper",
        "//utils/containers/lists:concurrent_list_trit_array",
    ],
)

cc_library(
    name = "responder_shared",
    hdrs = ["responder.h"],
    deps = ["//utils/handles:thread"],
)

cc_library(
    name = "responder",
    srcs = ["responder.c"],
    hdrs = ["responder.h"],
    deps = [
        ":responder_shared",
        "//ciri:core_shared",
        "//gossip:neighbor_shared",
        "//utils/containers/queues:concurrent_queue_packet",
        "//utils/containers/queues:concurrent_queue_transaction_request",
    ],
)
