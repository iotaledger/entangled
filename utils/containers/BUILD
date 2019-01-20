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

cc_library(
    name = "dummy_dep",
)
