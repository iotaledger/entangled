package(default_visibility = ["//visibility:public"])

cc_library(
    name = "conf_args",
    hdrs = ["conf_args.h"],
)

cc_library(
    name = "conf_default",
    hdrs = ["conf_default.h"],
)

cc_library(
    name = "conf",
    srcs = [
        "conf.c",
        "conf_provider_getopt.c",
    ],
    hdrs = ["conf.h"],
    deps = [
        ":conf_args",
        ":conf_default",
        "//common:errors",
    ],
)
