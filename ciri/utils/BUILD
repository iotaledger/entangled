package(default_visibility = ["//visibility:public"])

cc_library(
    name = "files",
    srcs = ["files.c"],
    hdrs = ["files.h"],
    deps = [
        "//common:errors",
    ],
)

cc_library(
    name = "signed_files",
    srcs = ["signed_files.c"],
    hdrs = ["signed_files.h"],
    deps = [
        "//common:errors",
        "//common/crypto/curl-p:trit",
        "//common/crypto/iss:normalize",
        "//common/crypto/iss/v1:iss_curl",
        "//common/crypto/kerl",
        "//common/model:bundle",
        "//common/trinary:trit_long",
        "//common/trinary:tryte_ascii",
        "//utils:merkle",
    ],
)
