cc_library(
    name = "api",
    srcs = [
        "iota_api.c",
    ],
    hdrs = ["iota_api.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":service",
        "//cclient/http:beast",
    ],
)

cc_library(
    name = "service",
    srcs = [
        "service.c",
    ],
    hdrs = ["service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//cclient/serialization:serializer_json",
    ],
)
