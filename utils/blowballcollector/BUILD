cc_library(
    name='blowballcollector',
    include_prefix='iota/utils',
    hdrs=glob(['*.hpp']),
    srcs=['blowballcollector.cpp'],
    deps=[
        '//utils/common:common',
        '//utils/prometheus_collector:prometheus_collector',
        '@com_github_gflags_gflags//:gflags',
        '@glog//:glog',
	    '@prometheus_cpp//:prometheus_cpp',
    ],
    visibility = ["//visibility:public"])