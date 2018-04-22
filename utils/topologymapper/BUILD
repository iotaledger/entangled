cc_library(
    name='topologymapper',
    include_prefix='iota/utils',
    hdrs=glob(['*.hpp']),
    srcs=['topologymapper.cpp'],
    deps=[
        '//utils/common:common',
        '//utils/prometheus_collector:prometheus_collector',
        '@com_github_gflags_gflags//:gflags',
        '@glog//:glog',
	    '@prometheus_cpp//:prometheus_cpp',
    ],
    visibility = ["//visibility:public"])