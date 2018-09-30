package(default_visibility = ["//visibility:public"])

cc_library(
    name = "usage",
    srcs = ["usage.c"],
    hdrs = ["usage.h"],
)

cc_library(
    name = "conf_values",
    hdrs = ["conf_values.h"],
)

cc_library(
    name = "conf",
    srcs = [
        "conf.c",
        "conf_provider_getopt.c",
    ],
    hdrs = ["conf.h"],
    deps = [
        ":conf_values",
        ":usage",
        "//common:errors",
        "//utils:logger_helper",
    ],
)
