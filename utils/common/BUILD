cc_library(
    name='common',
    include_prefix='iota/utils/common',
    hdrs=glob(['*.hpp']),
    copts=['-DZMQ_BUILD_DRAFT_API'],
    srcs=glob(['*.cpp', '*.hpp']),
    deps=[
        '@rxcpp//:rxcpp',
        '@cppzmq//:cppzmq',
        '@cpprestsdk//:cpprestsdk',
    ],
    visibility=['//visibility:public'])

cc_test(
    name='common_test',
    srcs=['tests/iri.cpp'],
    deps=[
        ':common',
        '@gtest//:gtest_main',
        '@glog//:glog',
        '@com_github_gflags_gflags//:gflags',
    ], )
