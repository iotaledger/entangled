package(default_visibility = ["//visibility:public"])

load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

cc_library(
    name = "message",
    srcs = ["message.c"],
    hdrs = ["message.h"],
    deps = [
        ":channel",
        ":endpoint",
        "//mam/v2:defs",
        "//mam/v2/mss",
        "//mam/v2/ntru",
        "//mam/v2/ntru:mam_ntru_pk_t_set",
        "//mam/v2/ntru:mam_ntru_sk_t_set",
        "//mam/v2/pb3",
        "//mam/v2/prng",
        "//mam/v2/psk",
    ],
)

cc_library(
    name = "channel_hdr",
    hdrs = ["channel.h"],
    deps = [
        ":mam_endpoint_t_set",
        "//mam/v2/mss",
        "//mam/v2/trits",
    ],
)

cc_library(
    name = "channel",
    srcs = ["channel.c"],
    deps = [
        ":channel_hdr",
        ":endpoint",
        ":mam_channel_t_set",
        "//mam/v2/pb3",
    ],
)

cc_library(
    name = "endpoint_hdr",
    hdrs = ["endpoint.h"],
    deps = [
        "//mam/v2/mss",
        "//mam/v2/trits",
    ],
)

cc_library(
    name = "endpoint",
    srcs = ["endpoint.c"],
    deps = [
        ":endpoint_hdr",
        ":mam_endpoint_t_set",
        "//mam/v2/pb3",
    ],
)

typed_container_generate(
    additional_deps = ":endpoint_hdr",
    additional_include_path = "mam/v2/mam/endpoint.h",
    container_type = "set",
    parent_directory = "mam/v2/mam",
    value_type = "mam_endpoint_t",
)

typed_container_generate(
    additional_deps = "channel_hdr",
    additional_include_path = "mam/v2/mam/channel.h",
    container_type = "set",
    parent_directory = "mam/v2/mam",
    value_type = "mam_channel_t",
)
