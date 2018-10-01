package(default_visibility = ["//visibility:public"])

cc_library(
    name = "export",
    hdrs = ["export.h"],
)

cc_library(
    name = "files",
    srcs = ["files.c"],
    hdrs = ["files.h"],
    deps = [
        ":logger_helper",
        "//common:errors",
    ],
)

cc_library(
    name = "logger_helper",
    srcs = ["logger_helper.c"],
    hdrs = ["logger_helper.h"],
    copts = ["-DLOGGER_ENABLE"],
    deps = [
        "//utils/handles:lock",
        "@com_github_embear_logger//:logger",
    ],
)

cc_library(
    name = "macros",
    hdrs = ["macros.h"],
)

cc_library(
    name = "system",
    srcs = ["system.c"],
    hdrs = ["system.h"],
)

cc_library(
    name = "time",
    srcs = ["time.c"],
    hdrs = ["time.h"],
)

cc_library(
    name = "traversal",
    hdrs = ["traversal.h"],
)
