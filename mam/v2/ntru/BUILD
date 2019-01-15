package(default_visibility = ["//visibility:public"])

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
