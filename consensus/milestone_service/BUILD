cc_library(
    name = "milestone_service",
    srcs = [
        "milestone_service.c",
        "milestone_service.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:milestone",
        "//consensus:conf",
        "//consensus/snapshot",
        "//consensus/tangle",
    ],
)
