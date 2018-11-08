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
    data = [
        "db/ciri-mainnet.db",
        "db/ciri-testnet.db",
    ],
    deps = [
        ":core",
        "//utils/containers/lists:concurrent_list_neighbor",
    ],
)

cc_library(
    name = "core_shared",
    hdrs = ["core.h"],
    deps = [
        ":conf",
        "//ciri/api",
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
        ":mainnet": CIRI_MAINNET_VARIABLES,
        ":testnet": CIRI_TESTNET_VARIABLES,
        "//conditions:default": CIRI_MAINNET_VARIABLES,
    }),
    deps = [
        ":usage",
        "//ciri/api:conf",
        "//common:errors",
        "//consensus:conf",
        "//gossip:conf",
        "//utils:logger_helper",
    ],
)
