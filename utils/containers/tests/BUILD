cc_test(
    name = "test_map",
    timeout = "short",
    srcs = ["test_map.c"],
    deps = [
        "//utils/containers:person_example_t_to_int_map",
        "//utils/containers/hash/tests:defs",
        "@unity",
    ],
)

cc_test(
    name = "test_set",
    timeout = "short",
    srcs = ["test_set.c"],
    deps = [
        "//utils/containers:int_set",
        "@unity",
    ],
)
