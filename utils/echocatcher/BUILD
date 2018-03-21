cc_library(
    name='echocatcher',
    include_prefix='iota/utils/echocatcher',
    hdrs=glob(['*.hpp']),
    srcs=['echocatcher.cpp'],
    deps=[
        '//utils/common:common',
        '//ccurl:ccurl',
        '//utils/exposer:exposer',
        '@cpprestsdk//:cpprestsdk',
        '@com_github_gflags_gflags//:gflags',
        '@glog//:glog',
	'@prometheus_cpp//:prometheus_cpp',
    ],
    visibility = ["//visibility:public"])

