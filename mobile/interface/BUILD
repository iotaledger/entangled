cc_library(
    name="interface",
    deps=[
        "//common/curl-p:hashcash",
        "//common/trinary:trit_tryte"
    ],
    srcs=["bindings.c"],
    hdrs=["bindings.h"],
    visibility = ["//visibility:public"])

cc_test(name='tests',
        srcs=glob(['tests/**/*.cpp', '**/*.hpp']),
        deps=['@gtest//:gtest_main', '@gtest//:gtest', ':interface'])
