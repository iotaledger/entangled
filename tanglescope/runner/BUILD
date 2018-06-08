cc_library(name='shared',
           srcs=glob(['**/*.cpp'],
                     exclude=['tanglescope.cpp', 'tests/**/*.cpp']),
           hdrs=glob(
               ['**/*.hpp'],
               exclude=['tests/**/*.hpp']),
           deps=[
               '//tanglescope/echocatcher:echocatcher',
               '//tanglescope/statscollector:statscollector',
               '//tanglescope/blowballcollector:blowballcollector',
               '@yaml_cpp//:yaml_cpp',
           ])


cc_binary(name='tanglescope',
          srcs=["runner.cpp"],
          deps=[':shared'],
          data = [":copy_configuration",],
)

cc_test(name='tests',
        srcs=glob(['tests/**/*.cpp', '**/*.hpp']),
        deps=['@gtest//:gtest_main', '@gtest//:gtest',
              ':shared',],
        timeout="short")

genrule(name = 'copy_configuration',
        local = 1,
        srcs = glob(['configuration.yaml']),
        outs = ["default_configuration.yaml"],
        cmd = "cp $(SRCS) $(@)",
        output_to_bindir = 1)
