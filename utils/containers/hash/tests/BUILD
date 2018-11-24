cc_library(
    name = "defs",
    hdrs = ["defs.h"],
)

cc_test(
    name = "test_hash_queue",
    srcs = ["test_hash_queue.c"],
    deps = [
        ":defs",
        "//utils/containers/hash:hash243_queue",
        "@unity",
    ],
)

cc_test(
    name = "test_hash_stack",
    srcs = ["test_hash_stack.c"],
    deps = [
        ":defs",
        "//utils/containers/hash:hash243_stack",
        "@unity",
    ],
)

cc_test(
    name = "test_hash_array",
    srcs = ["test_hash_array.c"],
    deps = [
        ":defs",
        "//utils/containers/hash:hash_array",
        "@unity",
    ],
)
