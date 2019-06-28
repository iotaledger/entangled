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
        "//ciri/node:neighbor",
        "//ciri/node:node_shared",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "processor_shared",
    hdrs = ["processor.h"],
    deps = [
        "//ciri/consensus/transaction_validator",
        "//ciri/node:iota_packet",
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
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/transaction_solidifier",
        "//ciri/node:neighbor",
        "//ciri/node:node_shared",
        "//common/crypto/curl-p:ptrit",
        "//common/trinary:trit_ptrit",
    ],
)

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = ["//ciri/node/services:receiver_shared"],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.c"],
    deps = [
        ":receiver_shared",
        "//ciri/node:node_shared",
        "//ciri/node/services:receiver",
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
        "//ciri/consensus/tangle",
        "//ciri/node:neighbor",
        "//ciri/node:node_shared",
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
        "//ciri/consensus/tangle",
        "//ciri/node:node_shared",
        "//utils:logger_helper",
        "//utils:time",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "responder_shared",
    hdrs = ["responder.h"],
    deps = [
        "//ciri/node:transaction_request",
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
        "//ciri/consensus/tangle",
        "//ciri/node:neighbor_shared",
        "//ciri/node:node_shared",
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
        "//ciri/consensus/tangle",
        "//ciri/node:iota_packet",
        "//ciri/node:node_shared",
        "//common/model:milestone",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "tips_solidifier_shared",
    hdrs = ["tips_solidifier.h"],
    deps = [
        "//ciri/node:conf",
        "//ciri/node:tips_cache",
        "//common:errors",
        "//utils/handles:cond",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "tips_solidifier",
    srcs = ["tips_solidifier.c"],
    deps = [
        ":tips_solidifier_shared",
        "//ciri/consensus/tangle",
        "//ciri/consensus/transaction_solidifier",
        "//utils:logger_helper",
    ],
)
