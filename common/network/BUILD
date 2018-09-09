package(default_visibility = ["//visibility:public"])

cc_library(
    name = "endpoint",
    hdrs = ["endpoint.h"],
    deps = [":network"],
)

cc_library(
    name = "network",
    hdrs = ["network.h"],
)

cc_library(
    name = "uri_parser",
    srcs = ["uri_parser.c"],
    hdrs = ["uri_parser.h"],
    deps = [":network"],
)
