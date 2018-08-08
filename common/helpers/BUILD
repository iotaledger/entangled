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

cc_binary(
    name = "helpers.dll",
    linkshared = 1,
    linkstatic = 1,
    deps = [":helpers"],
)

cc_library(
    name = "sign",
    srcs = ["sign.c"],
    hdrs = ["sign.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/kerl",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:trit_tryte",
        "//utils:export",
        "//common/trinary:trit_array",
    ],
)

cc_library(
    name = "sign_3",
    srcs = ["sign.c"],
    hdrs = ["sign.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":pow",
        "//common/kerl",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:trit_tryte",
        "//common/trinary:trit_array_3",
    ],
)

cc_library(
    name = "sign_4",
    srcs = ["sign.c"],
    hdrs = ["sign.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":pow",
        "//common/kerl",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:trit_tryte",
        "//common/trinary:trit_array_4",
    ],
)

cc_library(
    name = "sign_5",
    srcs = ["sign.c"],
    hdrs = ["sign.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":pow",
        "//common/kerl",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:trit_tryte",
        "//common/trinary:trit_array_5",
    ],
)

cc_library(
    name = "pow",
    srcs = ["pow.c"],
    hdrs = ["pow.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/pow",
        "//utils:export",
    ],
)

cc_library(
    name = "digest",
    srcs = ["digest.c"],
    hdrs = ["digest.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/curl-p:digest",
        "//utils:export",
    ],
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
        "//utils:export",
    ],
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

cc_test(
    name = "tests_sign_3",
    timeout = "short",
    srcs = glob([
        "tests/**/test_sign.cpp",
        "**/*.hpp",
    ]),
    tags = ["exclusive"],
    deps = [
        ":sign_3",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "tests_sign_4",
    timeout = "short",
    srcs = glob([
        "tests/**/test_sign.cpp",
        "**/*.hpp",
    ]),
    tags = ["exclusive"],
    deps = [
        ":sign_4",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "tests_sign_5",
    timeout = "short",
    srcs = glob([
        "tests/**/test_sign.cpp",
        "**/*.hpp",
    ]),
    tags = ["exclusive"],
    deps = [
        ":sign_5",
        "@com_google_googletest//:gtest_main",
    ],
)
