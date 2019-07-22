package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster_shared",
    hdrs = ["broadcaster.h"],
    deps = [
        "//ciri/node/protocol:gossip",
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
        "//ciri/node:node_shared",
        "//ciri/node/network:neighbor",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "processor_shared",
    hdrs = ["processor.h"],
    deps = [
        "//ciri/consensus/transaction_validator",
        "//ciri/node/protocol:gossip",
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
        "//ciri/node:node_shared",
        "//ciri/node/network:neighbor",
        "//common/crypto/curl-p:ptrit",
        "//common/trinary:trit_ptrit",
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
        "//ciri/node/protocol:transaction_request",
        "//utils/handles:cond",
        "//utils/handles:lock",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "responder",
    srcs = ["responder.c"],
    deps = [
        ":responder_shared",
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/tangle",
        "//ciri/node:node_shared",
        "//ciri/node/network:neighbor_shared",
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
        "//ciri/node:node_shared",
        "//ciri/node/protocol:gossip",
        "//common/model:milestone",
        "//utils:logger_helper",
    ],
)
