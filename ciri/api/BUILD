package(default_visibility = ["//visibility:public"])

load("//ciri:conf.bzl", "CIRI_MAINNET_VARIABLES")
load("//ciri:conf.bzl", "CIRI_TESTNET_VARIABLES")

cc_library(
    name = "api",
    srcs = ["api.c"],
    hdrs = ["api.h"],
    defines = select({
        "//config:mainnet": CIRI_MAINNET_VARIABLES,
        "//config:testnet": CIRI_TESTNET_VARIABLES,
        "//conditions:default": CIRI_MAINNET_VARIABLES,
    }),
    deps = [
        ":conf",
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//ciri:core_shared",
        "//common:errors",
        "//consensus",
        "//gossip/components:broadcaster",
        "//utils:logger_helper",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "http",
    srcs = ["http.c"],
    hdrs = ["http.h"],
    deps = [
        ":api",
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//cclient/serialization:serializer",
        "//cclient/serialization:serializer_json",
        "//common:errors",
        "//common/storage",
        "//utils:logger_helper",
        "//utils/handles:thread",
        "@cJSON",
        "@libmicrohttpd",
    ],
)

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    deps = ["//common:errors"],
)
