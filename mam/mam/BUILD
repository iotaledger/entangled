package(default_visibility = ["//visibility:public"])

load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

cc_library(
    name = "message",
    srcs = ["message.c"],
    hdrs = ["message.h"],
    deps = [
        ":channel",
        ":endpoint",
        ":mam_pk_t_set",
        "//mam:defs",
        "//mam/mss",
        "//mam/ntru",
        "//mam/pb3",
        "//mam/prng",
        "//mam/psk",
        "//mam/sponge:spongos_types",
    ],
)

cc_library(
    name = "channel_hdr",
    hdrs = ["channel.h"],
    deps = [
        ":mam_endpoint_t_set",
        "//mam/mss",
        "//mam/trits",
    ],
)

cc_library(
    name = "channel",
    srcs = ["channel.c"],
    deps = [
        ":channel_hdr",
        ":endpoint",
        ":mam_channel_t_set",
        "//mam/pb3",
    ],
)

cc_library(
    name = "endpoint_hdr",
    hdrs = ["endpoint.h"],
    deps = [
        "//mam/mss",
        "//mam/trits",
    ],
)

cc_library(
    name = "endpoint",
    srcs = ["endpoint.c"],
    deps = [
        ":endpoint_hdr",
        ":mam_endpoint_t_set",
        "//mam/pb3",
    ],
)

typed_container_generate(
    additional_deps = ":endpoint_hdr",
    additional_include_path = "mam/mam/endpoint.h",
    container_type = "set",
    parent_directory = "mam/mam",
    value_type = "mam_endpoint_t",
)

typed_container_generate(
    additional_deps = "channel_hdr",
    additional_include_path = "mam/mam/channel.h",
    container_type = "set",
    parent_directory = "mam/mam",
    value_type = "mam_channel_t",
)

typed_container_generate(
    additional_deps = "channel_hdr",
    additional_include_path = "mam/mam/channel.h",
    container_type = "set",
    parent_directory = "mam/mam",
    value_type = "mam_pk_t",
)
