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
        ":serializer",
        "@cJSON",
        "@com_github_uthash//:uthash",
    ],
    alwayslink = True,
)

cc_library(
    name = "serializer",
    hdrs = glob([
        "serializer.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
    ],
)
