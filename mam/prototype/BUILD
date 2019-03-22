package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mam",
    srcs = ["mam.c"],
    hdrs = ["mam.h"],
    deps =
        [
            ":mask",
            "//common/crypto/curl-p:hamming",
            "//common/trinary:trit_long",
            "//utils:merkle",
        ],
)

cc_library(
    name = "mask",
    srcs = ["mask.c"],
    hdrs = ["mask.h"],
    deps =
        [
            "//common/crypto/curl-p:trit",
            "//common/trinary:add",
        ],
)
