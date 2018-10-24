package(default_visibility = ["//visibility:public"])

cc_library(
    name = "api",
    srcs = ["api.c"],
    hdrs = ["api.h"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//cclient/serialization:serializer_json",
        "//common:errors",
        "//consensus/tangle",
        "//utils:logger_helper",
        "//utils/handles:thread",
    ],
)
