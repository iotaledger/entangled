package(default_visibility = ["//visibility:public"])

cc_library(
    name = "api",
    srcs = ["api.c"],
    hdrs = ["api.h"],
    deps = [
        ":conf",
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//cclient/serialization:serializer_json",
        "//common:errors",
        "//consensus/tangle",
        "//consensus/transaction_validator",
        "//gossip/components:broadcaster",
        "//utils:logger_helper",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    deps = ["//common:errors"],
)
