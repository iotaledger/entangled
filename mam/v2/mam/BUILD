package(default_visibility = ["//visibility:public"])

load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

cc_library(
    name = "mam",
    srcs = ["mam.c"],
    hdrs = ["mam.h"],
    deps = [
        ":alloc",
        ":channel",
        ":mam_ntru_pk_t_set",
        ":mam_pre_shared_key_t_set",
        "//mam/v2:defs",
        "//mam/v2/mss",
        "//mam/v2/ntru",
        "//mam/v2/pb3",
        "//mam/v2/prng",
    ],
)

cc_library(
    name = "alloc",
    hdrs = ["alloc.h"],
    deps = [
        "//mam/v2/sponge",
    ],
)

cc_library(
    name = "channel",
    srcs = ["channel.c"],
    hdrs = ["channel.h"],
    deps = [
        ":alloc",
        "//mam/v2/mss",
        "//mam/v2/pb3",
        "//mam/v2/trits",
    ],
)

cc_library(
    name = "mam_types_hdr",
    hdrs = ["mam_types.h"],
    deps = [
        "//mam/v2:defs",
        "//mam/v2/ntru",
        "//mam/v2/trits",
    ],
)

cc_library(
    name = "mam_types",
    srcs = ["mam_types.c"],
    deps = [
        ":mam_ntru_pk_t_set",
        ":mam_pre_shared_key_t_set",
        ":mam_types_hdr",
        "//mam/v2:defs",
        "//mam/v2/ntru",
        "//mam/v2/trits",
    ],
)

typed_container_generate(
    additional_deps = ":mam_types_hdr",
    additional_include_path = "mam/v2/mam/mam_types.h",
    container_type = "set",
    parent_directory = "mam/v2/mam",
    value_type = "mam_ntru_pk_t",
)

typed_container_generate(
    additional_deps = ":mam_types_hdr",
    additional_include_path = "mam/v2/mam/mam_types.h",
    container_type = "set",
    parent_directory = "mam/v2/mam",
    value_type = "mam_pre_shared_key_t",
)
