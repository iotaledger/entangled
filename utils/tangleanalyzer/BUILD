cc_library(name='shared',
           srcs=glob(['**/*.cpp'],
                     exclude=['tangleanalyzer.cpp', 'tests/**/*.cpp']),
           hdrs=glob(
               ['**/*.hpp'],
               exclude=['tests/**/*.hpp']),
           deps=[
               '//utils/common:common',
               '@com_github_gflags_gflags//:gflags',
               '@glog//:glog',
           ])

cc_binary(name='tangleanalyzer',
          srcs=["tangleanalyzer.cpp"],
          deps=['@yaml_cpp//:yaml_cpp',
                '//utils/echocatcher:echocatcher',
                '//utils/statscollector:statscollector'],

          data = [":copy_configuration",],
)

genrule(name = 'copy_configuration',
        local = 1,
        srcs = glob(['configuration.yaml']),
        outs = ["default_configuration.yaml"],
        cmd = "cp $(SRCS) $(@)",
        output_to_bindir = 1)
