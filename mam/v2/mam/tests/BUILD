cc_test(
    name = "test_message",
    timeout = "long",
    srcs = ["test_message.c"],
    deps = [
        "//mam/v2/mam:message",
        "//mam/v2/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_endpoint",
    timeout = "long",
    srcs = ["test_endpoint.c"],
    deps = [
        "//mam/v2/mam:endpoint",
        "//mam/v2/mam:mam_endpoint_t_set",
        "//mam/v2/mam:message",
        "//mam/v2/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_channel",
    timeout = "long",
    srcs = ["test_channel.c"],
    deps = [
        "//mam/v2/mam:channel",
        "//mam/v2/mam:mam_channel_t_set",
        "//mam/v2/mam:message",
        "//mam/v2/test_utils",
        "@unity",
    ],
)
