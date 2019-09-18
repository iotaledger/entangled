cc_library(
    name = "test_utils",
    visibility = ["//visibility:public"],
    deps = [
        ":bundle",
        ":spent_addresses",
        ":tangle",
    ],
)

cc_library(
    name = "tangle",
    srcs = ["tangle.c"],
    hdrs = ["tangle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/tangle",
        "//common:errors",
        "//common/storage",
        "//common/storage:test_utils",
        "//utils:files",
    ],
)

cc_library(
    name = "spent_addresses",
    srcs = ["spent_addresses.c"],
    hdrs = ["spent_addresses.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/spent_addresses:spent_addresses_provider",
        "//common:errors",
        "//common/storage",
        "//common/storage:test_utils",
        "//utils:files",
    ],
)

cc_library(
    name = "bundle",
    srcs = ["bundle.c"],
    hdrs = ["bundle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/tangle",
        "//common:errors",
        "//common/helpers:digest",
        "//common/helpers:pow",
        "//common/helpers:sign",
        "//common/model:bundle",
        "//utils:macros",
        "//utils:time",
    ],
)
