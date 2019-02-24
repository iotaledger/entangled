cc_binary(
    name = "send",
    srcs = ["send.c"],
    deps = [
        "//cclient:api",
        "//mam/v2/api",
    ],
)

cc_binary(
    name = "recv",
    srcs = ["recv.c"],
    deps = [
        ":commmon",
        "//cclient:api",
        "//mam/v2/api",
    ],
)

cc_library(
    name = "commmon",
    hdrs = ["common.h"],
    deps = [
        "//common/trinary:trit_tryte",
    ],
)
