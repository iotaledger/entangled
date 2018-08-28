package(default_visibility = ["//visibility:public"])

cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "config_args",
    hdrs = ["config_args.h"],
)

cc_library(
    name = "config_default",
    hdrs = ["config_default.h"],
)

cc_library(
    name = "conf_provider_shared",
    hdrs = ["provider.h"],
    deps = [
        ":config",
        "//common:errors",
    ],
)

cc_library(
    name = "conf_provider",
    srcs = ["getopt.c"],
    deps = [
        ":conf_provider_shared",
        ":config_args",
        ":config_default",
    ],
)
