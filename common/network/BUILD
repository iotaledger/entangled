package(default_visibility = ["//visibility:public"])

cc_library(
    name = "iota_packet",
    hdrs = ["iota_packet.h"],
    deps = [
        ":socket",
        "//common/trinary:tryte",
    ],
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

cc_library(
    name = "lock_handle",
    hdrs = ["lock_handle.h"],
)

cc_library(
    name = "cond_handle",
    hdrs = ["cond_handle.h"],
)
