package(default_visibility = ["//visibility:public"])

cc_library(
    name = "network",
    hdrs = ["network.h"],
)

cc_library(
    name = "iota_packet",
    hdrs = ["iota_packet.h"],
    deps = [
        ":endpoint",
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
    name = "neighbor",
    srcs = ["neighbor.c"],
    hdrs = ["neighbor.h"],
    deps = [
        ":iota_packet",
        ":uri_parser",
        "//common/network/services:tcp_sender",
        "//common/network/services:udp_sender",
        "//common/trinary:trit_array",
    ],
)

cc_library(
    name = "endpoint",
    hdrs = ["endpoint.h"],
    deps = [":network"],
)
