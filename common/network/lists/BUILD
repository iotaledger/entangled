package(default_visibility = ["//visibility:public"])

cc_library(
    name = "concurrent_list",
    srcs = ["concurrent_list.c.inc"],
    hdrs = ["concurrent_list.h.inc"],
)

cc_library(
    name = "concurrent_list_neighbor",
    srcs = ["concurrent_list_neighbor.c"],
    hdrs = ["concurrent_list_neighbor.h"],
    deps = [
        ":concurrent_list",
        "//common/network:neighbor",
    ],
)
