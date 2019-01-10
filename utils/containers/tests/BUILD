
load("//utils/containers:map_generator.bzl", "map_generate")


map_generate(
    additional_deps = ["//utils/containers:person_example"],
    additional_include_path = "utils/containers/person_example.h",
    key_type = "person_example_t",
    parent_directory = "utils/containers",
    value_type = "int",
)



cc_test(
    name = "test_map",
    srcs = ["test_map.c"],
    deps = [
        ":person_example_t_to_int_map",
        "//utils/containers/hash/tests:defs",
        "@unity",
    ],
)

cc_test(
    name = "test_set",
    srcs = ["test_set.c"],
    deps = [
        "//utils/containers:int_set",
        "@unity",
    ],
)
