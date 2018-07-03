cc_library(
    name = "cclient",
    srcs = glob([
        "*.c",
        "**/*.c",
        "**/*.h",
    ]),
    hdrs = ["iota_api.h"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
        "//cclient/serialization:serializer_json",
        "@com_google_googletest//:gtest_main",
    ],
)
