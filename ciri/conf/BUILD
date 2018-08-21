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
    name = "conf_provider",
    hdrs = ["provider.h"],
    srcs = ["getopt.c"],
    deps = [
        "//common:errors",
        ":config",
        ":config_args",
    ],
)
