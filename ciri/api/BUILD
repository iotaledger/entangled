package(default_visibility = ["//visibility:public"])

load("//ciri:conf.bzl", "CIRI_MAINNET_VARIABLES")
load("//ciri:conf.bzl", "CIRI_TESTNET_VARIABLES")

cc_library(
    name = "api",
    srcs = ["api.c"],
    hdrs = ["api.h"],
    defines = select({
        "//ciri:mainnet": CIRI_MAINNET_VARIABLES,
        "//ciri:testnet": CIRI_TESTNET_VARIABLES,
        "//conditions:default": CIRI_MAINNET_VARIABLES,
    }),
    deps = [
        ":conf",
        ":feature",
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//ciri:core",
        "//common:errors",
        "//common/helpers:pow",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    deps = [
        "//ciri/utils:files",
        "//common:errors",
    ],
)

cc_library(
    name = "feature",
    srcs = ["feature.c"],
    hdrs = ["feature.h"],
    deps = ["@com_github_uthash//:uthash"],
)
