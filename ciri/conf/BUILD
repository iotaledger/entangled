package(default_visibility = ["//visibility:public"])

cc_library(
    name = "usage",
    srcs = ["usage.c"],
    hdrs = ["usage.h"],
)

cc_library(
    name = "conf",
    srcs = [
        "conf.c",
        "conf_provider_getopt.c",
    ],
    hdrs = ["conf.h"],
    deps = [
        ":usage",
        "//ciri/api:conf",
        "//common:errors",
        "//consensus:conf",
        "//gossip:conf",
        "//utils:logger_helper",
    ],
)
