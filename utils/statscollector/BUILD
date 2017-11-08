cc_binary(
    name='statscollector',
    srcs=glob(['**/*.cpp', '**/*.hpp']),
    deps=[
        '//utils/common:common',
        '@com_github_gflags_gflags//:gflags',
        '@glog//:glog',
        '@influxdb_cpp_rest//:influxdb_cpp_rest',
    ])
