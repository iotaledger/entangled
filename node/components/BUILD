package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster_shared",
    hdrs = ["broadcaster.h"],
    deps = [
        "//utils/containers/hash:hash8019_queue",
        "//utils/handles:cond",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "broadcaster",
    srcs = ["broadcaster.c"],
    deps = [
        ":broadcaster_shared",
        "//node:neighbor",
        "//node:node_shared",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "processor_shared",
    hdrs = ["processor.h"],
    deps = [
        "//consensus/transaction_validator",
        "//node:iota_packet",
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
        "//common/crypto/curl-p:ptrit",
        "//common/trinary:trit_ptrit",
        "//consensus/milestone:milestone_tracker",
        "//consensus/transaction_solidifier",
        "//node:neighbor",
        "//node:node_shared",
    ],
)

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = ["//node/services:receiver_shared"],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.c"],
    deps = [
        ":receiver_shared",
        "//node:node_shared",
        "//node/services:receiver",
    ],
)

cc_library(
    name = "transaction_requester_worker_shared",
    hdrs = ["transaction_requester_worker.h"],
    deps = [
        ":transaction_requester_shared",
        "//common:errors",
    ],
)

cc_library(
    name = "transaction_requester_worker",
    srcs = ["transaction_requester_worker.c"],
    deps = [
        ":transaction_requester_worker_shared",
        "//consensus/tangle",
        "//node:neighbor",
        "//node:node_shared",
        "//utils:logger_helper",
        "//utils:time",
    ],
)

cc_library(
    name = "transaction_requester_shared",
    hdrs = ["transaction_requester.h"],
    deps = [
        "//common:errors",
        "//utils/containers/hash:hash243_set",
        "//utils/containers/hash:hash243_stack",
        "//utils/handles:cond",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "transaction_requester",
    srcs = ["transaction_requester.c"],
    deps = [
        ":transaction_requester_shared",
        "//consensus/tangle",
        "//node:node_shared",
        "//utils:logger_helper",
        "//utils:time",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "responder_shared",
    hdrs = ["responder.h"],
    deps = [
        "//node:transaction_request",
        "//utils/handles:cond",
        "//utils/handles:lock",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "responder",
    srcs = ["responder.c"],
    hdrs = ["responder.h"],
    deps = [
        ":responder_shared",
        "//consensus/tangle",
        "//node:neighbor_shared",
        "//node:node_shared",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "tips_requester_shared",
    hdrs = ["tips_requester.h"],
    deps = [
        "//common:errors",
        "//utils/handles:cond",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "tips_requester",
    srcs = ["tips_requester.c"],
    deps = [
        ":tips_requester_shared",
        "//common/model:milestone",
        "//consensus/tangle",
        "//node:iota_packet",
        "//node:node_shared",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "tips_solidifier_shared",
    hdrs = ["tips_solidifier.h"],
    deps = [
        "//common:errors",
        "//node:conf",
        "//node:tips_cache",
        "//utils/handles:cond",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "tips_solidifier",
    srcs = ["tips_solidifier.c"],
    deps = [
        ":tips_solidifier_shared",
        "//consensus/tangle",
        "//consensus/transaction_solidifier",
        "//utils:logger_helper",
    ],
)
