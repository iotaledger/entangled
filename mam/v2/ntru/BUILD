package(default_visibility = ["//visibility:public"])

config_setting(
    name = "mred_binary",
    values = {"define": "mred_binary=1"},
)

config_setting(
    name = "no_mred_binary",
    values = {"define": "mred_binary=0"},
)

cc_library(
    name = "ntru",
    srcs = ["ntru.c"],
    hdrs = ["ntru.h"],
    deps = [
        ":poly",
        "//mam/v2/prng",
        "//mam/v2/sponge:spongos",
    ],
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
    defines = ["MAM2_POLY_MRED_BINARY"],
    deps = [
        "//mam/v2:defs",
        "//mam/v2/trits",
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
        "//mam/v2:defs",
        "//mam/v2/trits",
    ],
)
