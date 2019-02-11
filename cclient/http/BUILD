cc_library(
    name = "shared",
    hdrs = ["http.h"],
    visibility = ["//visibility:public"],
    deps = ["//cclient:service"],
)

cc_library(
    name = "http",
    srcs = [
        "http.c",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        "//utils/handles:socket",
        "@http_parser",
    ],
)
