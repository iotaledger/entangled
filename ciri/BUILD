package(default_visibility = ["//visibility:public"])

load(":conf.bzl", "CIRI_MAINNET_VARIABLES")
load(":conf.bzl", "CIRI_TESTNET_VARIABLES")

config_setting(
    name = "mainnet",
    values = {"define": "network=mainnet"},
)

config_setting(
    name = "testnet",
    values = {"define": "network=testnet"},
)

cc_binary(
    name = "ciri",
    srcs = ["main.c"],
    copts = ["-DLOGGER_ENABLE"],
    data = select({
        "//ciri:mainnet": ["db/ciri-mainnet.db"],
        "//ciri:testnet": ["db/ciri-testnet.db"],
        "//conditions:default": ["db/ciri-mainnet.db"],
    }) + [":conf.yml"],
    deps = [
        ":core",
        "//ciri/api",
        "//ciri/api:http",
        "//utils/handles:rand",
        "//utils/handles:signal",
    ],
)

cc_library(
    name = "core_shared",
    hdrs = ["core.h"],
    deps = [
        ":conf",
        "//consensus",
        "//gossip:node_shared",
    ],
)

cc_library(
    name = "core",
    srcs = ["core.c"],
    deps = [
        ":core_shared",
        "//gossip:node",
    ],
)

cc_library(
    name = "usage",
    srcs = ["usage.c"],
    hdrs = ["usage.h"],
)

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    defines = select({
        "//ciri:mainnet": CIRI_MAINNET_VARIABLES,
        "//ciri:testnet": CIRI_TESTNET_VARIABLES,
        "//conditions:default": CIRI_MAINNET_VARIABLES,
    }),
    deps = [
        ":usage",
        "//ciri/api:conf",
        "//common:errors",
        "//consensus:conf",
        "//gossip:conf",
        "//utils:logger_helper",
        "@yaml",
    ],
)
