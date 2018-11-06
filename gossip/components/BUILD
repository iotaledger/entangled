package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster_shared",
    hdrs = ["broadcaster.h"],
    deps = [
        "//utils/containers/hash:hash8019_queue",
        "//utils/handles:cond",
        "//utils/handles:lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "broadcaster",
    srcs = ["broadcaster.c"],
    deps = [
        ":broadcaster_shared",
        "//gossip:node_shared",
        "//utils:logger_helper",
        "//utils/containers/lists:concurrent_list_neighbor",
    ],
)

cc_library(
    name = "processor_shared",
    hdrs = ["processor.h"],
    deps = [
        "//consensus/transaction_solidifier",
        "//consensus/transaction_validator",
        "//utils/handles:cond",
        "//utils/handles:lock",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "processor",
    srcs = ["processor.c"],
    deps = [
        ":processor_shared",
        "//gossip:node_shared",
        "//utils/containers/lists:concurrent_list_neighbor",
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
        "//gossip:node_shared",
        "//gossip/services:receiver",
    ],
)

cc_library(
    name = "transaction_requester",
    srcs = ["transaction_requester.c"],
    hdrs = ["transaction_requester.h"],
    deps = [
        "//common:errors",
        "//consensus/tangle",
        "//gossip:conf",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_set",
        "//utils/handles:rw_lock",
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
        "//gossip:neighbor_shared",
        "//gossip:node_shared",
        "//utils/containers/queues:concurrent_queue_transaction_request",
    ],
)
