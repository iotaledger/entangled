cc_library(name='shared',
           srcs=glob(['**/*.cpp'],
                     exclude=['tangleanalyzer.cpp', 'tests/**/*.cpp']),
           hdrs=glob(
               ['**/*.hpp'],
               exclude=['tests/**/*.hpp']),
           deps=[
               '//utils/echocatcher:echocatcher',
               '//utils/statscollector:statscollector',
               '@yaml_cpp//:yaml_cpp',
           ])


cc_binary(name='tangleanalyzer',
          srcs=["tangleanalyzer.cpp"],
          deps=[
                ':shared',],
          data = [":copy_configuration",],
)

cc_test(name='tests',
        srcs=glob(['tests/**/*.cpp', '**/*.hpp']),
        deps=['@gtest//:gtest_main', '@gtest//:gtest',
              ':shared',])

genrule(name = 'copy_configuration',
        local = 1,
        srcs = glob(['configuration.yaml']),
        outs = ["default_configuration.yaml"],
        cmd = "cp $(SRCS) $(@)",
        output_to_bindir = 1)
