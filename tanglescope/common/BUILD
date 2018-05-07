cc_library(
    name='common',
    include_prefix='iota/tanglescope/common',
    hdrs=glob(['*.hpp']),
    defines=['ZMQ_BUILD_DRAFT_API'],
    srcs=glob(['*.cpp', '*.hpp']),
    deps=[
        '@rxcpp//:rxcpp',
        '@cppzmq//:cppzmq',
        '@glog//:glog',
        '@cpprestsdk//:cpprestsdk',
	'@libcuckoo//:libcuckoo',
	'@yaml_cpp//:yaml_cpp',
    ],
    visibility=['//visibility:public'])

cc_test(
    name='common_test',
    srcs=['tests/iri.cpp'],
    deps=[
        ':common',
        '@gtest//:gtest_main',
        '@com_github_gflags_gflags//:gflags',
    ], )
