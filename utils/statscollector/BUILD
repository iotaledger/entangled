cc_library(name='shared',
           srcs=glob(['**/*.cpp'],
                     exclude=['statscollector.cpp','tests/**/*.cpp']),
           hdrs=glob(
               ['**/*.hpp'],
               exclude=['tests/**/*.hpp']),
           include_prefix='iota/utils/statscollector',
           deps=[
               '//utils/common:common',
               '@com_github_gflags_gflags//:gflags',
               '@glog//:glog',
           ])

cc_test(name='tests',
        srcs=glob(['tests/**/*.cpp', '**/*.hpp']),
        deps=['@gtest//:gtest_main', '@gtest//:gtest', ':shared'])

cc_library(name='statscollector',
           include_prefix='iota/utils',
           hdrs=glob(['statscollector.hpp']),
           srcs=["statscollector.cpp"],
           deps=[
	       '@prometheus_cpp//:prometheus_cpp',
           ':shared',
           '//utils/exposer:exposer',
	       ],
           visibility = ["//visibility:public"]
      )
