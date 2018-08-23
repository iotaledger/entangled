package(default_visibility = ["//visibility:public"])

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = ["//common/network"],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.cc"],
    deps = [
        ":receiver_shared",
        "//gossip/services:tcp_receiver",
        "//gossip/services:udp_receiver",
        "//utils/containers/queues:concurrent_queue_packet",
    ],
)

cc_library(
    name = "tcp_receiver",
    srcs = ["tcp_receiver.cc"],
    hdrs = ["tcp_receiver.hpp"],
    deps = [
        "//ciri:node_shared",
        "//utils:logger_helper",
        "//utils/containers/lists:concurrent_list_neighbor",
        "//utils/containers/queues:concurrent_queue_packet",
        "@boost//:asio",
    ],
)

cc_library(
    name = "tcp_sender",
    srcs = ["tcp_sender.cc"],
    hdrs = ["tcp_sender.hpp"],
    deps = [
        "//common/network:iota_packet",
        "//utils:logger_helper",
        "@boost//:asio",
    ],
)

cc_library(
    name = "udp_receiver",
    srcs = ["udp_receiver.cc"],
    hdrs = ["udp_receiver.hpp"],
    deps = [
        "//common/network:neighbor",
        "//utils:logger_helper",
        "//utils/containers/lists:concurrent_list_neighbor",
        "//utils/containers/queues:concurrent_queue_packet",
        "@boost//:asio",
    ],
)

cc_library(
    name = "udp_sender",
    srcs = ["udp_sender.cc"],
    hdrs = ["udp_sender.hpp"],
    deps = [
        "//common/network:iota_packet",
        "//utils:logger_helper",
        "@boost//:asio",
    ],
)
