package(default_visibility = ["//visibility:public"])

load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

cc_library(
    name = "psk_hdr",
    hdrs = ["psk.h"],
    deps = [
        "//common:errors",
        "//mam/trits",
    ],
)

cc_library(
    name = "psk",
    srcs = ["psk.c"],
    deps = [
        ":mam_psk_t_set",
        ":psk_hdr",
        "//mam/pb3",
        "//mam/prng",
        "//utils:memset_safe",
    ],
)

typed_container_generate(
    additional_deps = ":psk_hdr",
    additional_include_path = "mam/psk/psk.h",
    container_type = "set",
    parent_directory = "mam/psk",
    value_type = "mam_psk_t",
)
