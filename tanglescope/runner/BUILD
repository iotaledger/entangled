cc_library(
    name = "shared",
    srcs = glob(
        ["**/*.cpp"],
        exclude = [
            "tanglescope.cpp",
            "tests/**/*.cpp",
        ],
    ),
    hdrs = glob(
        ["**/*.hpp"],
        exclude = ["tests/**/*.hpp"],
    ),
    deps = [
        "//tanglescope:blowballcollector",
        "//tanglescope:confirmationratecollector",
        "//tanglescope:echocollector",
        "//tanglescope:tanglewidthcollector",
        "//tanglescope:tipselectioncollector",
        "//tanglescope/statscollector",
        "@yaml_cpp",
    ],
)

cc_binary(
    name = "tanglescope",
    srcs = ["runner.cpp"],
    data = [":copy_configuration"],
    deps = [":shared"],
)

cc_test(
    name = "tests",
    timeout = "short",
    srcs = glob([
        "tests/**/*.cpp",
        "**/*.hpp",
    ]),
    deps = [
        ":shared",
        "@com_google_googletest//:gtest_main",
    ],
)

genrule(
    name = "copy_configuration",
    srcs = glob(["configuration.yaml"]),
    outs = ["default_configuration.yaml"],
    cmd = "cp $(SRCS) $(@)",
    local = 1,
    output_to_bindir = 1,
)
