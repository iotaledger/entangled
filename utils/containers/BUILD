load(":map_generator.bzl", "map_generate")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bitset",
    srcs = ["bitset.c"],
    hdrs = ["bitset.h"],
)

map_generate(
    key_type = "double",
    value_type = "double",
)
