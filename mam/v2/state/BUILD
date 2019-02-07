package(default_visibility = ["//visibility:public"])

cc_library(
    name = "state",
    srcs = ["state.c"],
    hdrs = ["state.h"],
    deps = [
        "//common:errors",
        "//mam/v2/mam:mam_pre_shared_key_t_set",
        "//mam/v2/mam:mam_types",
        "//mam/v2/ntru:mam_ntru_pk_t_set",
        "//mam/v2/ntru:mam_ntru_sk_t_set",
    ],
)
