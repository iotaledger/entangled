package(default_visibility = ["//visibility:public"])

load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

config_setting(
    name = "mred_binary",
    values = {"define": "mred_binary=1"},
)

config_setting(
    name = "no_mred_binary",
    values = {"define": "mred_binary=0"},
)

cc_library(
    name = "ntru_types_hdr",
    hdrs = ["ntru_types.h"],
    deps = [
        ":poly",
        "//mam:defs",
    ],
)

cc_library(
    name = "ntru",
    srcs = ["ntru.c"],
    hdrs = ["ntru.h"],
    deps = [
        ":mam_ntru_pk_t_set",
        ":mam_ntru_sk_t_set",
        ":ntru_types_hdr",
        "//mam/prng",
        "//mam/sponge:spongos",
        "//utils:memset_safe",
    ],
)

typed_container_generate(
    additional_deps = ":ntru_types_hdr",
    additional_include_path = "mam/ntru/ntru_types.h",
    container_type = "set",
    parent_directory = "mam/ntru",
    value_type = "mam_ntru_pk_t",
)

typed_container_generate(
    additional_deps = ":ntru_types_hdr",
    additional_include_path = "mam/ntru/ntru_types.h",
    container_type = "set",
    parent_directory = "mam/ntru",
    value_type = "mam_ntru_sk_t",
)

cc_library(
    name = "poly",
    deps =
        select({
            ":mred_binary": [":poly_mred_binary"],
            ":no_mred_binary": [":poly_no_mred_binary"],
            "//conditions:default": [":poly_no_mred_binary"],
        }),
)

cc_library(
    name = "poly_mred_binary",
    srcs = ["poly.c"],
    hdrs = [
        "poly.h",
        "poly_param.h",
    ],
    defines = ["MAM_POLY_MRED_BINARY"],
    deps = [
        "//mam:defs",
        "//mam/trits",
    ],
)

cc_library(
    name = "poly_no_mred_binary",
    srcs = ["poly.c"],
    hdrs = [
        "poly.h",
        "poly_param.h",
    ],
    deps = [
        "//mam:defs",
        "//mam/trits",
    ],
)
