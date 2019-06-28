package(default_visibility = ["//visibility:public"])

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = [
        "//ciri/node/pipeline:processor_shared",
        "//common/network",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.cc"],
    deps = [
        ":receiver_shared",
        "//ciri/node/services:tcp_receiver",
        "//ciri/node/services:udp_receiver",
    ],
)

cc_library(
    name = "tcp_receiver",
    srcs = ["tcp_receiver.cc"],
    hdrs = ["tcp_receiver.hpp"],
    deps = [
        "//ciri/node:neighbor",
        "//utils:logger_helper",
        "@boost//:asio",
        "@boost//:crc",
    ],
)

cc_library(
    name = "tcp_sender",
    srcs = ["tcp_sender.cc"],
    hdrs = ["tcp_sender.hpp"],
    deps = [
        ":receiver_shared",
        "//ciri/node:iota_packet",
        "//utils:logger_helper",
        "@boost//:asio",
        "@boost//:crc",
    ],
)

cc_library(
    name = "udp_receiver",
    srcs = ["udp_receiver.cc"],
    hdrs = ["udp_receiver.hpp"],
    deps = [
        ":receiver_shared",
        "@boost//:asio",
    ],
)

cc_library(
    name = "udp_sender",
    srcs = ["udp_sender.cc"],
    hdrs = ["udp_sender.hpp"],
    deps = [
        ":receiver_shared",
        "//ciri/node:iota_packet",
        "//utils:logger_helper",
        "@boost//:asio",
    ],
)
