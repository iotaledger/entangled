package(default_visibility = ["//visibility:public"])

config_setting(
    name = "merkle_traversal",
    values = {"define": "merkle_mode=traversal"},
)

config_setting(
    name = "merkle_classic",
    values = {"define": "merkle_mode=classic"},
)

cc_library(
    name = "mss",
    deps =
        select({
            ":merkle_traversal": [":mss_traversal"],
            ":merkle_classic": [":mss_classic"],
            "//conditions:default": [":mss_traversal"],
        }),
)

cc_library(
    name = "mss_traversal",
    srcs = [
        "mss_common.c",
        "mss_traversal.c",
    ],
    hdrs = [
        "mss.h",
        "mss_common.h",
        "mss_traversal.h",
    ],
    defines = ["MAM_MSS_TRAVERSAL"],
    deps = [
        "//common:errors",
        "//mam/wots",
    ],
)

cc_library(
    name = "mss_classic",
    srcs = [
        "mss_classic.c",
        "mss_common.c",
    ],
    hdrs = [
        "mss.h",
        "mss_classic.h",
        "mss_common.h",
    ],
    deps = [
        "//common:errors",
        "//mam/wots",
    ],
)
