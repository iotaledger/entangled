package(default_visibility = ["//visibility:public"])

cc_library(
    name = "iss_curl",
    srcs = [
        "iss.c.inc",
        "iss_curl.c",
    ],
    hdrs = [
        "iss.h.inc",
        "iss_curl.h",
    ],
    deps = [
        "//common/curl-p:trit",
        "//common/trinary:add",
    ],
)

cc_library(
    name = "iss_kerl",
    srcs = [
        "iss.c.inc",
        "iss_kerl.c",
    ],
    hdrs = [
        "iss.h.inc",
        "iss_kerl.h",
    ],
    deps = [
        "//common/kerl",
        "//common/trinary:add",
    ],
)
