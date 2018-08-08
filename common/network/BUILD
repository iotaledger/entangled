package(default_visibility = ["//visibility:public"])

cc_library(
    name = "network",
    hdrs = ["network.h"],
)

cc_library(
    name = "iota_packet",
    hdrs = ["iota_packet.h"],
    deps = [":endpoint"],
)

cc_library(
    name = "logger",
    srcs = ["logger.c"],
    hdrs = ["logger.h"],
    deps = ["//utils/handles:lock"],
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
    ],
)

cc_library(
    name = "endpoint",
    hdrs = ["endpoint.h"],
    deps = [":network"],
)
