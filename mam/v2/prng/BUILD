package(default_visibility = ["//visibility:public"])

load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

cc_library(
    name = "prng",
    srcs = ["prng.c"],
    hdrs = ["prng.h"],
    deps = [
        ":prng_types",
        "//common:errors",
        "//mam/v2:defs",
        "//mam/v2/sponge",
        "//mam/v2/trits",
    ],
)

cc_library(
    name = "prng_types",
    hdrs = ["prng_types.h"],
    deps = [
        "//mam/v2:defs",
        "//mam/v2/sponge",
        "//mam/v2/trits",
    ],
)

typed_container_generate(
    additional_deps = ":mam_types",
    additional_include_path = "mam/v2/mam/mam_types.h",
    container_type = "set",
    parent_directory = "mam/v2/mam",
    value_type = "mam_ntru_pk_t",
)
