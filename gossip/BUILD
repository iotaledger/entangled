package(default_visibility = ["//visibility:public"])

cc_library(
    name = "iota_packet",
    srcs = ["iota_packet.c"],
    hdrs = ["iota_packet.h"],
    deps = [
        "//ciri/conf:conf_values",
        "//common:errors",
        "//common/model:transaction",
        "//common/network:endpoint",
        "//common/trinary:bytes",
    ],
)

cc_library(
    name = "neighbor_shared",
    hdrs = ["neighbor.h"],
    deps = [
        ":iota_packet",
        "//common:errors",
    ],
)

cc_library(
    name = "neighbor",
    srcs = ["neighbor.c"],
    deps = [
        ":neighbor_shared",
        ":node_shared",
        "//common/model:transaction",
        "//common/network:uri_parser",
        "//common/trinary:trit_array",
        "//gossip/components:transaction_requester",
        "//gossip/services:tcp_sender",
        "//gossip/services:udp_sender",
    ],
)

cc_library(
    name = "transaction_request",
    hdrs = ["transaction_request.h"],
)

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    deps = ["//common:errors"],
)

cc_library(
    name = "node_shared",
    hdrs = ["node.h"],
    deps = [
        "//gossip/components:broadcaster_shared",
        "//gossip/components:processor_shared",
        "//gossip/components:receiver_shared",
        "//gossip/components:responder_shared",
        "//gossip/components:transaction_requester",
    ],
)

cc_library(
    name = "node",
    srcs = ["node.c"],
    deps = [
        ":node_shared",
        "//gossip/components:broadcaster",
        "//gossip/components:processor",
        "//gossip/components:receiver",
        "//gossip/components:responder",
    ],
)
