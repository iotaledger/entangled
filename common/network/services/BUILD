package(default_visibility = ["//visibility:public"])

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = ["//common/network/queues:concurrent_queue_packet"],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.cc"],
    deps = [
        "//common/network/services:tcp_receiver",
        "//common/network/services:udp_receiver",
    ],
)

cc_library(
    name = "udp_receiver",
    srcs = ["udp_receiver.cc"],
    hdrs = ["udp_receiver.hpp"],
    deps = [
        "//common/network:logger",
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
        "//ciri:node_shared",
        "//common/network:logger",
        "//common/network:neighbor",
        "//common/network/components:receiver_shared",
        "//common/network/services:receiver_shared",
        "@boost//:asio",
    ],
)

cc_library(
    name = "tcp_sender",
    srcs = ["tcp_sender.cc"],
    hdrs = ["tcp_sender.hpp"],
    deps = [
        "//common/network:endpoint",
        "//common/network:logger",
        "@boost//:asio",
    ],
)

cc_library(
    name = "udp_sender",
    srcs = ["udp_sender.cc"],
    hdrs = ["udp_sender.hpp"],
    deps = [
        "//common/network:endpoint",
        "//common/network:logger",
        "@boost//:asio",
    ],
)
