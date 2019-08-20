cc_test(
    name = "bundle_validator",
    timeout = "moderate",
    srcs = ["bundle_validator.c"],
    data = ["//common/storage:tangle_db"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/bundle_validator",
        "//ciri/consensus/test_utils",
        "//common/storage/tests/helpers",
        "@unity",
    ],
)
