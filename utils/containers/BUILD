load(":map_generator.bzl", "map_generate")

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
    key_type = "double",
    value_type = "double",
)

map_generate(
    key_type = "person_example_t",
    value_type = "int",
)
