package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "ciri",
    srcs = ["main.c"],
    copts = ["-DLOGGER_ENABLE"],
    deps = [
        ":core",
    ],
)

cc_library(
    name = "core_shared",
    hdrs = ["core.h"],
    deps = [
        ":node_shared",
        "//common/storage/sql/sqlite3:sqlite3_storage",
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
        "//common:errors",
        "//common/network/components:broadcaster_shared",
        "//common/network/components:processor",
        "//common/network/components:receiver_shared",
        "//common/network/components:requester_shared",
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
        "//common/network/components:requester",
    ],
)
