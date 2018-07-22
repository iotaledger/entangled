package(default_visibility = ["//visibility:public"])

cc_library(
    name = "receiver",
    srcs = ["receiver.cpp"],
    hdrs = ["receiver.h"],
    deps = [
        "//common/network:logger",
        "//common/network/services:tcp_receiver",
        "//common/network/services:udp_receiver",
    ],
)

cc_library(
    name = "udp_receiver",
    srcs = ["udp_receiver.cpp"],
    hdrs = ["udp_receiver.hpp"],
    deps = [
        "//common/network/components:receiver_shared",
        "@boost//:asio",
    ],
)

cc_library(
    name = "tcp_receiver",
    srcs = ["tcp_receiver.cpp"],
    hdrs = ["tcp_receiver.hpp"],
    deps = [
        "//common/network/components:receiver_shared",
        "@boost//:asio",
    ],
)
