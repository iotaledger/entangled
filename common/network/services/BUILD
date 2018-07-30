package(default_visibility = ["//visibility:public"])

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.cc"],
    deps = [
        ":receiver_shared",
        "//common/network:logger",
        "//common/network/services:tcp_receiver",
        "//common/network/services:udp_receiver",
    ],
)

cc_library(
    name = "udp_receiver",
    srcs = ["udp_receiver.cc"],
    hdrs = ["udp_receiver.hpp"],
    deps = [
        "//common/network/components:receiver_shared",
        "//common/network/services:receiver_shared",
        "@boost//:asio",
    ],
)

cc_library(
    name = "tcp_receiver",
    srcs = ["tcp_receiver.cc"],
    hdrs = ["tcp_receiver.hpp"],
    deps = [
        "//common/network/components:receiver_shared",
        "//common/network/services:receiver_shared",
        "@boost//:asio",
    ],
)
