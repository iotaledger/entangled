package(default_visibility = ["//visibility:public"])

cc_library(
    name = "list",
    srcs = ["list.c.inc"],
    hdrs = ["list.h.inc"],
)

cc_library(
    name = "list_neighbor",
    srcs = ["list_neighbor.c"],
    hdrs = ["list_neighbor.h"],
    deps = [
        ":list",
        "//common/network:neighbor",
    ],
)
