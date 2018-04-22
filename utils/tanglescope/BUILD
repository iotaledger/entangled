load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

cc_library(name='shared',
           srcs=glob(['**/*.cpp'],
                     exclude=['tanglescope.cpp', 'tests/**/*.cpp']),
           hdrs=glob(
               ['**/*.hpp'],
               exclude=['tests/**/*.hpp']),
           deps=[
               '//utils/echocatcher:echocatcher',
               '//utils/statscollector:statscollector',
               '//utils/topologymapper:topologymapper',
               '@yaml_cpp//:yaml_cpp',
           ])


cc_binary(name='tanglescope',
          srcs=["tanglescope.cpp"],
          deps=[':shared'],
          data = [":copy_configuration",],
)

#need docker client installation:
#https://docs.docker.com/install/linux/docker-ce/ubuntu/#set-up-the-repository
cc_image(
    name='tanglescope_img',
    srcs=['tanglescope.cpp'],
    deps=[':shared'],
    data=[":copy_configuration"],
    base="@essentials//image"
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
