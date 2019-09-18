cc_binary(
    name = "send-header",
    srcs = ["send-header.c"],
    deps = [
        ":send-common",
    ],
)

cc_binary(
    name = "send-packet",
    srcs = ["send-packet.c"],
    deps = [
        ":send-common",
    ],
)

cc_binary(
    name = "send-msg",
    srcs = ["send-msg.c"],
    deps = [
        ":send-common",
    ],
)

cc_library(
    name = "send-common",
    srcs = ["send-common.c"],
    hdrs = ["send-common.h"],
    deps = [
        ":common",
    ],
)

cc_binary(
    name = "recv",
    srcs = ["recv.c"],
    deps = [
        ":common",
    ],
)

cc_library(
    name = "common",
    srcs = ["common.c"],
    hdrs = ["common.h"],
    deps = [
        "//cclient/api",
        "//common/trinary:trit_tryte",
        "//common/trinary:tryte_ascii",
        "//mam/api",
    ],
)

cc_binary(
    name = "ntru-key-exchange",
    srcs = ["ntru-key-exchange.c"],
    deps = [
        "//mam/api",
    ],
)
