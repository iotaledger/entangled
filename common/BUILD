package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stdint",
    hdrs = ["stdint.h"],
)

cc_library(
    name = "errors",
    hdrs = ["errors.h"],
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
