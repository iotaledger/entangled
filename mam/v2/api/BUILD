package(default_visibility = ["//visibility:public"])

cc_library(
    name = "api",
    srcs = ["api.c"],
    hdrs = ["api.h"],
    deps = [
        "//common:errors",
        "//common/model:bundle",
        "//mam/v2/mam",
        "//mam/v2/mam:mam_psk_t_set",
        "//mam/v2/mam:mam_types",
        "//mam/v2/ntru:mam_ntru_pk_t_set",
        "//mam/v2/ntru:mam_ntru_sk_t_set",
    ],
)
