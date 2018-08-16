package(default_visibility = ["//visibility:public"])

cc_library(
    name = "network",
    hdrs = ["network.h"],
)

cc_library(
    name = "hash_request",
    hdrs = ["hash_request.h"],
    deps = [
        ":neighbor_shared",
        "//common/trinary:trit_array",
    ],
)

cc_library(
    name = "iota_packet",
    srcs = ["iota_packet.c"],
    hdrs = ["iota_packet.h"],
    deps = [
        ":endpoint",
        "//common/model:transaction",
        "//common/trinary:bytes",
    ],
)

cc_library(
    name = "uri_parser",
    srcs = ["uri_parser.c"],
    hdrs = ["uri_parser.h"],
    deps = [":network"],
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
        ":uri_parser",
        "//ciri:node_shared",
        "//common/model:transaction",
        "//common/network/components:requester_shared",
        "//common/network/services:tcp_sender",
        "//common/network/services:udp_sender",
    ],
)

cc_library(
    name = "endpoint",
    hdrs = ["endpoint.h"],
    deps = [":network"],
)
