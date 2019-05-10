load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

cc_library(
    name = "psk_hdr",
    hdrs = ["psk.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//common:errors",
        "//mam/prng",
        "//mam/trits",
        "//utils:memset_safe",
    ],
)

cc_library(
    name = "psk",
    srcs = ["psk.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":mam_psk_t_set",
        ":psk_hdr",
        "//mam/pb3",
    ],
)

typed_container_generate(
    additional_deps = ":psk_hdr",
    additional_include_path = "mam/psk/psk.h",
    container_type = "set",
    parent_directory = "mam/psk",
    value_type = "mam_psk_t",
)
