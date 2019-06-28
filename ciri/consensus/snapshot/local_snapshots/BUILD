cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//utils:files",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "shared_local_snapshots",
    deps = [
        ":conf",
        "//ciri/consensus:conf",
        "//ciri/consensus/milestone:milestone_tracker",
        "//common:errors",
        "//utils/handles:cond",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "local_snapshots_manager",
    srcs = [
        "local_snapshots_manager.c",
    ],
    hdrs = ["local_snapshots_manager.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared_local_snapshots",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/snapshot:snapshots_service",
    ],
)
