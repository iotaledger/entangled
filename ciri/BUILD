package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "ciri",
    srcs = ["main.c"],
    copts = ["-DLOGGER_ENABLE"],
    data = ["ciri.db"],
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
    deps = [
        ":usage",
        "//ciri/api:conf",
        "//common:errors",
        "//consensus:conf",
        "//gossip:conf",
        "//utils:logger_helper",
    ],
)
