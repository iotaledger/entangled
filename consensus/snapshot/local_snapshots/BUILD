cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "shared_local_snapshots",
    deps = [
        ":conf",
        "//common:errors",
        "//consensus:conf",
        "//consensus/milestone_tracker",
        "//utils/handles:cond",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "local_snapshots_service",
    srcs = [
        "local_snapshots_service.c",
        "local_snapshots_service.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":shared_local_snapshots",
    ],
)

cc_library(
    name = "local_snapshots_provider",
    srcs = [
        "local_snapshots_provider.c",
        "local_snapshots_provider.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":shared_local_snapshots",
    ],
)

cc_library(
    name = "local_snapshots_manager",
    srcs = [
        "local_snapshots_manager.c",
        "local_snapshots_manager.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":local_snapshots_provider",
        ":local_snapshots_service",
        ":shared_local_snapshots",
    ],
)
