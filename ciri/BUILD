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
        "//ciri/api",
        "//ciri/conf",
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
