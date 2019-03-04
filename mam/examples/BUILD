cc_binary(
    name = "send",
    srcs = ["send.c"],
    deps = [
        ":commmon",
    ],
)

cc_binary(
    name = "recv",
    srcs = ["recv.c"],
    deps = [
        ":commmon",
    ],
)

cc_library(
    name = "commmon",
    srcs = ["common.c"],
    hdrs = ["common.h"],
    deps = [
        "//cclient:api",
        "//common/trinary:trit_tryte",
        "//common/trinary:tryte_ascii",
        "//mam/api",
    ],
)
