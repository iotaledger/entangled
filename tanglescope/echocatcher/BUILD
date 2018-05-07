cc_library(
    name='echocatcher',
    include_prefix='iota/tanglescope/echocatcher',
    hdrs=glob(['*.hpp']),
    srcs=['echocatcher.cpp'],
    deps=[
        '//tanglescope/common:common',
        '//common/helpers:helpers',
        '//tanglescope/prometheus_collector:prometheus_collector',
        '//common/trinary:tryte_long',
        '@cpprestsdk//:cpprestsdk',
        '@com_github_gflags_gflags//:gflags',
        '@glog//:glog',
	'@prometheus_cpp//:prometheus_cpp',
    ],
    visibility = ["//visibility:public"])

