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
        "//ciri:node_shared",
        "//common/model:transaction",
        "//common/network:uri_parser",
        "//common/trinary:trit_array",
        "//gossip/components:requester",
        "//gossip/services:tcp_sender",
        "//gossip/services:udp_sender",
    ],
)

cc_library(
    name = "transaction_request",
    hdrs = ["transaction_request.h"],
)
