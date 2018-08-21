cc_library(
    name = "api",
    srcs = glob([
        "*.c",
        "**/*.c",
        "**/*.h",
    ]),
    hdrs = ["iota_api.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cclient/http:beast",
    ],
)

cc_library(
    name = "service",
    srcs = [
        "service.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//cclient/serialization:serializer_json",
    ],
)
