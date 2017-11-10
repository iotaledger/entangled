cc_library(name='shared',
           srcs=glob(['**/*.cpp'],
                     exclude=['statscollector.cpp', 'tests/**/*.cpp']),
           hdrs=glob(
               ['**/*.hpp'],
               exclude=['tests/**/*.hpp']),
           deps=[
               '//utils/common:common',
               '@com_github_gflags_gflags//:gflags',
               '@glog//:glog',
           ])

cc_test(name='statscollector_tests',
        srcs=glob(['tests/**/*.cpp', 'tests/**/*.hpp']),
        deps=[':shared', '@gtest//:gtest_main'])

cc_binary(name='statscollector',
          srcs=glob(['**/*.cpp', '**/*.hpp']),
          deps=[
              '@influxdb_cpp_rest//:influxdb_cpp_rest', ':shared'
          ])
