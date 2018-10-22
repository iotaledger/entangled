cc_library(
    name = "api",
    srcs = [
        "iota_client_core_api.c",
        "iota_client_extended_api.c",
    ],
    hdrs = [
        "iota_client_core_api.h",
        "iota_client_extended_api.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":service",
        "//cclient/http:beast",
        "//common/helpers:sign",
        "//utils:hash_containers",
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
