package(default_visibility = ["//visibility:public"])

genrule(
    name = "snapshot_testnet",
    outs = ["snapshotTestnet.txt"],
    cmd = "wget -q https://raw.githubusercontent.com/iotaledger/iri/dev/src/main/resources/$$(basename $@) -O $@",
)

cc_binary(
    name = "ciri",
    srcs = ["main.c"],
    copts = ["-DLOGGER_ENABLE"],
    data = [
        "ciri.db",
        ":snapshot_testnet",
    ],
    deps = [
        ":core",
        "//utils/containers/lists:concurrent_list_neighbor",
    ],
)

cc_library(
    name = "core_shared",
    hdrs = ["core.h"],
    deps = [
        ":node_shared",
        "//ciri/api",
        "//ciri/conf",
        "//consensus/snapshot",
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
        "//gossip/components:broadcaster",
        "//gossip/components:processor",
        "//gossip/components:receiver",
        "//gossip/components:requester",
        "//gossip/components:responder",
    ],
)
