cc_library(
    name = "receiver",
    srcs = ["receiver.cpp"],
    hdrs = ["receiver.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/network:iota_packet",
        "//common/network:logger",
        "//common/network/components:receiver_shared",
        "@boost//:asio",
    ],
)
