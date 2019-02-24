load(":map_generator.bzl", "map_generate")
load(":typed_container_generator.bzl", "typed_container_generate")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bitset",
    srcs = ["bitset.c"],
    hdrs = ["bitset.h"],
)

cc_library(
    name = "person_example",
    hdrs = ["person_example.h"],
    deps = ["//common/trinary:flex_trit"],
)

map_generate(
    additional_deps = [":person_example"],
    additional_include_path = "utils/containers/person_example.h",
    key_type = "person_example_t",
    value_type = "int",
)

typed_container_generate(
    container_type = "set",
    value_type = "int",
)

cc_library(
    name = "dummy_dep",
)
