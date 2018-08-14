package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "ciri",
    srcs = ["main.c"],
    copts = ["-DLOGGER_ENABLE"],
    deps = [
        ":core",
        "//utils/containers/lists:concurrent_list_neighbor",
        "//ciri/conf:configuration",
        "//ciri/conf:config_args",
    ],
)

cc_library(
    name = "core_shared",
    hdrs = ["core.h"],
    deps = [
        ":node_shared",
        "//consensus/tangle",
    ],
)

cc_library(
    name = "core",
    srcs = ["core.c"],
    deps = [
        ":core_shared",
        ":node",
    ],
)

cc_library(
    name = "node_shared",
    hdrs = ["node.h"],
    deps = [
        "//gossip/components:broadcaster_shared",
        "//gossip/components:processor_shared",
        "//gossip/components:receiver_shared",
        "//gossip/components:requester_shared",
        "//gossip/components:responder_shared",
    ],
)

cc_library(
    name = "node",
    srcs = ["node.c"],
    deps = [
        ":node_shared",
        "//ciri/conf:configuration",
        "//gossip/components:broadcaster",
        "//gossip/components:processor",
        "//gossip/components:receiver",
        "//gossip/components:requester",
        "//gossip/components:responder",
    ],
)
