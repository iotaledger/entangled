package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mam",
    srcs = ["mam.c"],
    hdrs = ["mam.h"],
    deps =
        [
            ":mask",
            ":merkle",
            "//common/curl-p:hamming",
            "//common/trinary:trit_long",
        ],
)

cc_library(
    name = "mask",
    srcs = ["mask.c"],
    hdrs = ["mask.h"],
    deps =
        [
            "//common/curl-p:trit",
            "//common/trinary:add",
        ],
)

cc_library(
    name = "merkle",
    srcs = ["merkle.c"],
    hdrs = ["merkle.h"],
    deps = ["//common/sign/v2:iss_curl"],
)
