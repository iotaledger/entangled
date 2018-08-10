package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "ciri",
    srcs = ["main.c"],
    copts = ["-DLOGGER_ENABLE"],
    deps = [
        ":core",
        ":node",
    ],
)

cc_library(
    name = "core",
    srcs = ["core.c"],
    hdrs = ["core.h"],
    deps = [
        "//common/storage/sql/sqlite3:sqlite3_storage",
    ],
)

cc_library(
    name = "node_shared",
    hdrs = ["node.h"],
    deps = [
        "//common/network/components:broadcaster_shared",
        "//common/network/components:processor",
        "//common/network/components:receiver_shared",
        "//common/network/components:requester",
        "//common/network/components:responder",
        "//utils/containers/lists:concurrent_list_neighbor",
    ],
)

cc_library(
    name = "node",
    srcs = ["node.c"],
    deps = [
        ":node_shared",
        "//common/network/components:broadcaster",
        "//common/network/components:receiver",
    ],
)
