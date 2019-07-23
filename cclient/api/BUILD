cc_library(
    name = "api_core",
    srcs = glob([
        "core/*.c",
        "core/*.h",
    ]),
    visibility = ["//visibility:private"],
    deps = [
        "//cclient:service",
        "//cclient/http",
        "//common/helpers",
        "//common/model:bundle",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "api_extended",
    srcs = glob([
        "extended/*.c",
        "extended/*.h",
    ]),
    visibility = ["//visibility:private"],
    deps = [
        ":api_core",
        "//common/model:inputs",
        "//common/model:transfer",
        "//utils:time",
    ],
)

cc_library(
    name = "api",
    hdrs = glob([
        "core/core_api.h",
        "extended/extended_api.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":api_extended",
    ],
)

cc_binary(
    name = "libcclient.so",
    linkshared = True,
    visibility = ["//visibility:public"],
    deps = [":api"],
)
