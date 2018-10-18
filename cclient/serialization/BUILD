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

cc_test(
    name = "test_json",
    srcs = [
        "tests/test_json.c",
        "tests/test_json.h",
    ],
    deps = [
        ":serializer_json",
        "@unity",
    ],
)
