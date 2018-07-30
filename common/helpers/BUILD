cc_library(
    name = "helpers",
    visibility = ["//visibility:public"],
    deps = [
        ":checksum",
        ":digest",
        ":pow",
        ":sign",
    ],
)

cc_library(
    name = "sign",
    srcs = ["sign.c"],
    hdrs = ["sign.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":pow",
        "//common/kerl",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:trit_tryte",
    ],
)

cc_library(
    name = "pow",
    srcs = ["pow.c"],
    hdrs = ["pow.h"],
    visibility = ["//visibility:public"],
    deps = ["//common/pow"],
)

cc_library(
    name = "digest",
    srcs = ["digest.c"],
    hdrs = ["digest.h"],
    visibility = ["//visibility:public"],
    deps = ["//common/curl-p:digest"],
)

cc_library(
    name = "checksum",
    srcs = ["checksum.c"],
    hdrs = ["checksum.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":digest",
        "//common/kerl:hash",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:trit_tryte",
    ],
)

cc_library(
    name = "files",
    srcs = ["files.c"],
    hdrs = ["files.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "tests",
    timeout = "short",
    srcs = glob([
        "tests/**/*.cpp",
        "**/*.hpp",
    ]),
    tags = ["exclusive"],
    deps = [
        ":helpers",
        "@com_google_googletest//:gtest_main",
    ],
)
