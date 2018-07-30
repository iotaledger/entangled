package(default_visibility = ["//visibility:public"])

cc_library(
    name = "iota_packet",
    hdrs = ["iota_packet.h"],
    deps = [":socket"],
)

cc_library(
    name = "logger",
    srcs = ["logger.c"],
    hdrs = ["logger.h"],
    deps = ["//common:lock_handle"],
)

cc_library(
    name = "neighbor",
    hdrs = ["neighbor.h"],
    deps = [":iota_packet"],
)

cc_library(
    name = "socket",
    hdrs = ["socket.h"],
)
