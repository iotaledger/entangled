load(":map_generator.bzl", "map_generate")
load(":type_container_generator.bzl", "type_container_generate")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bitset",
    srcs = ["bitset.c"],
    hdrs = ["bitset.h"],
)

cc_library(
    name = "person_example",
    hdrs = ["person_example.h"],
)

map_generate(
    key_type = "person_example_t",
    value_type = "int",
)

type_container_generate(
    container_type = "set",
    value_type = "int",
)

type_container_generate(
    additional_deps = "//mam/v2/mam:mam_types",
    additional_include_path = "mam/v2/mam/mam_types.h",
    container_type = "set",
    value_type = "mam_pre_shared_key_t",
)

type_container_generate(
    additional_deps = "//mam/v2/mam:mam_types",
    additional_include_path = "mam/v2/mam/mam_types.h",
    container_type = "set",
    value_type = "mam_ntru_pk_t",
)

cc_library(
    name = "dummy_dep",
)
