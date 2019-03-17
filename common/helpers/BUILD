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
        "//common:defs",
        "//common/crypto/kerl",
        "//common/sign:normalize",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:flex_trit",
        "//common/trinary:trit_array",
        "//common/trinary:trit_tryte",
        "//utils:export",
        "//utils:memset_safe",
    ],
)

cc_library(
    name = "pow",
    srcs = ["pow.c"],
    hdrs = ["pow.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":digest",
        "//common/curl-p:hashcash",
        "//common/model:bundle",
        "//common/trinary:flex_trit",
        "//common/trinary:trit_tryte",
        "//utils:export",
        "//utils:time",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "digest",
    srcs = ["digest.c"],
    hdrs = ["digest.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:defs",
        "//common/curl-p:digest",
        "//common/trinary:flex_trit",
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
        "//common:defs",
        "//common/crypto/kerl:hash",
        "//common/sign/v1:iss_kerl",
        "//common/trinary:flex_trit",
        "//common/trinary:trit_tryte",
        "//utils:export",
    ],
)
