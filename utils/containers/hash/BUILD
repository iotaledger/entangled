load(":hash_container_generator.bzl", "hash_container_generate")
load(":hash_map_generator.bzl", "hash_map_generate")

# Sets

hash_container_generate(
    size = 27,
    type = "set",
)

hash_container_generate(
    size = 81,
    type = "set",
)

hash_container_generate(
    size = 243,
    type = "set",
)

hash_container_generate(
    size = 6561,
    type = "set",
)

hash_container_generate(
    size = 8019,
    type = "set",
)

# Stacks

hash_container_generate(
    size = 27,
    type = "stack",
)

hash_container_generate(
    size = 81,
    type = "stack",
)

hash_container_generate(
    size = 243,
    type = "stack",
)

hash_container_generate(
    size = 6561,
    type = "stack",
)

hash_container_generate(
    size = 8019,
    type = "stack",
)

# Queues

hash_container_generate(
    size = 27,
    type = "queue",
)

hash_container_generate(
    size = 81,
    type = "queue",
)

hash_container_generate(
    size = 243,
    type = "queue",
)

hash_container_generate(
    size = 6561,
    type = "queue",
)

hash_container_generate(
    size = 8019,
    type = "queue",
)

hash_map_generate(
    mapped_type = "int64_t",
)

hash_map_generate(
    mapped_type = "uint64_t",
)

hash_map_generate(
    mapped_type = "double",
)

cc_library(
    name = "hash_array",
    srcs = ["hash_array.c"],
    hdrs = ["hash_array.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/trinary:flex_trit",
        "@com_github_uthash//:uthash",
    ],
)
