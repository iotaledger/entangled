cc_library(
    name='common',
    include_prefix='iota/utils/common',
    hdrs=glob(['*.hpp']),
    copts=['-DZMQ_BUILD_DRAFT_API'],
    srcs=glob(['*.cpp', '*.hpp']),
    deps=[
        '//external:rxcpp',
        '//external:cppzmq',
        '//external:cpprestsdk',
    ],
    visibility=['//visibility:public'])

cc_test(
    name='iri-test',
    srcs=['tests/iri.cpp'],
    deps=[
        ':common',
        '//external:gtest_main',
        '//external:glog',
        '//external:gflags',
    ], )
