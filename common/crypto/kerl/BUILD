package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bigint",
    srcs = ["bigint.c"],
    hdrs = ["bigint.h"],
    deps = ["//common:stdint"],
)

cc_library(
    name = "converter",
    srcs = ["converter.c"],
    hdrs = ["converter.h"],
    deps = [
        ":bigint",
        "//common:defs",
        "//common:stdint",
        "//common/trinary:trits",
        "//utils:memset_safe",
    ],
)

cc_library(
    name = "kerl",
    srcs = ["kerl.c"],
    hdrs = ["kerl.h"],
    deps = [
        ":converter",
        "@keccak",
    ],
)

cc_library(
    name = "hash",
    srcs = ["hash.c"],
    hdrs = ["hash.h"],
    deps = [
        ":kerl",
        "//common/trinary:tryte",
    ],
)
