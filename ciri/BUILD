package(default_visibility = ["//visibility:public"])

load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")
load("@io_bazel_rules_docker//container:container.bzl", "container_image")
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
        "//ciri:mainnet": glob([
            "db/tangle-mainnet.db",
            "db/spent-addresses-mainnet.db",
        ]),
        "//ciri:testnet": glob([
            "db/tangle-testnet.db",
            "db/spent-addresses-testnet.db",
        ]),
        "//conditions:default": glob([
            "db/tangle-mainnet.db",
            "db/spent-addresses-mainnet.db",
        ]),
    }) + glob(["conf.yml"]),
    deps = [
        ":core",
        "//ciri/api",
        "//ciri/api/http",
        "//utils/handles:rand",
        "//utils/handles:signal",
    ],
)

cc_image(
    name = "ciri_base_image",
    binary = ":ciri",
)

container_image(
    name = "ciri_image",
    base = ":ciri_base_image",
    mode = "755",
    repository = "iotaledger",
    stamp = True,
    volumes = ["/app/ciri/ciri.runfiles/org_iota_entangled/ciri/db"],
)

cc_library(
    name = "core_shared",
    hdrs = ["core.h"],
    deps = [
        ":conf",
        "//ciri/consensus",
        "//ciri/node:node_shared",
    ],
)

cc_library(
    name = "core",
    srcs = ["core.c"],
    deps = [
        ":core_shared",
        "//ciri/node",
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
        "//ciri/consensus:conf",
        "//ciri/node:conf",
        "//common:errors",
        "//utils:logger_helper",
        "@yaml",
    ],
)
