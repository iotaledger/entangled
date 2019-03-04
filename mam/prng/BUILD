load("//utils/containers:typed_container_generator.bzl", "typed_container_generate")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "prng",
    srcs = ["prng.c"],
    hdrs = ["prng.h"],
    deps = [
        ":prng_types",
        "//common:errors",
        "//mam:defs",
        "//mam/sponge",
        "//mam/trits",
        "//utils:memset_safe",
    ],
)

cc_library(
    name = "prng_types_hdr",
    hdrs = ["prng_types.h"],
    deps = [
        "//common:errors",
        "//mam:defs",
        "//mam/sponge",
        "//mam/trits",
    ],
)

cc_library(
    name = "prng_types",
    srcs = ["prng_types.c"],
    hdrs = ["prng_types.h"],
    deps = [
        ":mam_prng_t_set",
        ":prng_types_hdr",
        "//mam/pb3",
    ],
)

typed_container_generate(
    additional_deps = ":prng_types_hdr",
    additional_include_path = "mam/prng/prng_types.h",
    container_type = "set",
    parent_directory = "mam/prng",
    value_type = "mam_prng_t",
)
