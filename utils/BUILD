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
    name = "signed_files",
    srcs = ["signed_files.c"],
    hdrs = ["signed_files.h"],
    deps = [
        "//common:errors",
        "//common/curl-p:trit",
        "//common/kerl",
        "//common/model:bundle",
        "//common/trinary:tryte_ascii",
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
    name = "hash_containers",
    srcs = ["hash_containers.c"],
    hdrs = ["hash_containers.h"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "@com_github_uthash//:uthash",
    ],
)
