package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sponge",
    srcs = ["sponge.c"],
    hdrs = ["sponge.h"],
    deps = [
        "//common/trinary:add",
        "//mam/v2:defs",
        "//mam/v2/trits",
        "//mam/v2/trits:buffers",
    ],
)

cc_library(
    name = "spongos",
    srcs = ["spongos.c"],
    hdrs = ["spongos.h"],
    deps = [
        ":sponge",
    ],
)
