cc_library(
    name = "serializer_json",
    srcs = glob([
        "json/*.c",
        "json/*.h",
    ]),
    hdrs = glob([
        "json/*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":serializer_base",
    ],
)

cc_library(
    name = "serializer_base",
    srcs = glob([
        "*.h",
    ]),
    hdrs = glob([
        "*.h",
    ]),
    include_prefix = "serializer",
    visibility = ["//visibility:public"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
    ],
)
