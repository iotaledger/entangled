package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mam",
    srcs = ["mam.c"],
    hdrs = ["mam.h"],
    deps = [
        "//mam/v2:defs",
        "//mam/v2/ntru",
        "//mam/v2/pb3",
        "//mam/v2/prng",
        "//utils/containers:mam_ntru_pk_t_set",
        "//utils/containers:mam_pre_shared_key_t_set",
    ],
)

cc_library(
    name = "mam_types",
    hdrs = ["mam_types.h"],
    deps = [
        "//mam/v2:defs",
        "//mam/v2/trits",
    ],
)
