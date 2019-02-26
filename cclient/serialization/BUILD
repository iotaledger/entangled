cc_library(
    name = "serializer_json",
    srcs = glob([
        "json/*.c",
    ]),
    hdrs = glob([
        "json/*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":serializer_base",
        "@cJSON",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "serializer_base",
    srcs = glob([
        "serializer.h",
    ]),
    hdrs = glob([
        "serializer.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
    ],
)
