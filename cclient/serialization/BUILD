cc_library(
    name = "serializer_json",
    srcs = glob([
        "json/*.c",
        "json/*.h",
    ]),
    hdrs = glob([
        "json/*.h",
    ]),
    include_prefix = "serializer",
    visibility = ["//visibility:public"],
    deps = [
        ":serializer_base",
        "@cJSON",
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

cc_test(
    name = "test_json",
    srcs = [
        "tests/test_json.c",
        "tests/test_json.h",
    ],
    deps = [
        ":serializer_base",
        ":serializer_json",
        "@unity",
    ],
)
