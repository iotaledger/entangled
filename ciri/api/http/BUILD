package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http",
    srcs = ["http.c"],
    hdrs = ["http.h"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//cclient/serialization:serializer",
        "//cclient/serialization:serializer_json",
        "//ciri/api",
        "//ciri/storage:storage_common",
        "//common:errors",
        "//utils:logger_helper",
        "//utils/handles:thread",
        "@cJSON",
        "@libmicrohttpd",
    ],
)
