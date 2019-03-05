cc_binary(
    name = "send",
    srcs = ["send.c"],
    deps = [
        ":commmon",
        "//cclient/api",
        "//common/trinary:tryte_ascii",
        "//mam/api",
    ],
)

cc_binary(
    name = "recv",
    srcs = ["recv.c"],
    deps = [
        ":commmon",
        "//cclient/api",
        "//common/trinary:tryte_ascii",
        "//mam/api",
    ],
)

cc_library(
    name = "commmon",
    hdrs = ["common.h"],
    deps = [
        "//common/trinary:trit_tryte",
    ],
)
