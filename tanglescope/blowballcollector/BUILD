cc_library(
    name='blowballcollector',
    include_prefix='iota/tanglescope',
    hdrs=glob(['*.hpp']),
    srcs=['blowballcollector.cpp'],
    deps=[
        '//tanglescope/common:common',
        '//tanglescope/prometheus_collector:prometheus_collector',
        '@com_github_gflags_gflags//:gflags',
        '@glog//:glog',
	    '@prometheus_cpp//:prometheus_cpp',
    ],
    visibility = ["//visibility:public"])