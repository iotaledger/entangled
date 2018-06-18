cc_library(
    name = "serializer_json",
    srcs = glob([
        "json/*.c",
        "json/*.h",
    ]),
    hdrs = glob([
        "json/*.h",
    ]),

                deps = [
                    ":serializer_base",
                ],

    visibility = ["//visibility:public"],
)

cc_library(
    name = "serializer_base",
    srcs = glob([
        "*.h",
    ]),
    hdrs = glob([
        "*.h",
    ]),

            deps = [
                "//cclient/request:requests",
                "//cclient/response:responses",
            ],

    include_prefix = "serializer",
    visibility = ["//visibility:public"],
)
