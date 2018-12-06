package(default_visibility = ["//visibility:public"])

cc_library(
    name = "concurrent_list_trit_array",
    srcs = ["concurrent_list_trit_array.c"],
    hdrs = ["concurrent_list_trit_array.h"],
    deps = [
        ":concurrent_list",
        "//common/trinary:trit_array",
    ],
)

cc_library(
    name = "concurrent_list",
    srcs = ["concurrent_list.c.inc"],
    hdrs = ["concurrent_list.h.inc"],
    deps = ["//utils/handles:lock"],
)
