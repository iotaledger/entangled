cc_binary(
    name = "ciri",
    srcs = ["main.c"],
    deps = [":node"],
)

cc_library(
    name = "node",
    srcs = ["node.c"],
    hdrs = ["node.h"],
    deps = [
        "//common/network/components:broadcaster",
        "//common/network/components:processor",
        "//common/network/components:receiver",
        "//common/network/components:requester",
        "//common/network/components:responder",
    ],
)
