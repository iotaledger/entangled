package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sponge",
    srcs = ["sponge.c"],
    hdrs = ["sponge.h"],
    deps = [
        "//common:errors",
        "//common/trinary:add",
        "//mam:defs",
        "//mam/trits",
        "//mam/trits:buffers",
        "//mam/troika",
    ],
)

cc_library(
    name = "spongos",
    srcs = ["spongos.c"],
    hdrs = ["spongos.h"],
    deps = [
        ":sponge",
        ":spongos_types_hdr",
    ],
)

cc_library(
    name = "spongos_types_hdr",
    hdrs = ["spongos_types.h"],
    deps = [
        ":sponge",
    ],
)

cc_library(
    name = "spongos_types",
    srcs = ["spongos_types.c"],
    hdrs = ["spongos_types.h"],
    deps = [
        ":sponge",
        "//mam/pb3",
    ],
)
