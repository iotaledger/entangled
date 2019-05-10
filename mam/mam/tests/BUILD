cc_library(
    name = "test_channel_utils",
    srcs = ["test_channel_utils.c"],
    hdrs = ["test_channel_utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//mam/mam:channel",
        "//mam/mam:mam_channel_t_set",
    ],
)

cc_test(
    name = "test_message",
    timeout = "eternal",
    srcs = ["test_message.c"],
    deps = [
        "//mam/mam:message",
        "@unity",
    ],
)

cc_test(
    name = "test_endpoint",
    timeout = "long",
    srcs = ["test_endpoint.c"],
    deps = [
        "//mam/mam:endpoint",
        "//mam/mam:mam_endpoint_t_set",
        "//mam/mam:message",
        "@unity",
    ],
)

cc_test(
    name = "test_channel",
    timeout = "long",
    srcs = ["test_channel.c"],
    deps = [
        ":test_channel_utils",
        "//mam/mam:message",
        "@unity",
    ],
)
