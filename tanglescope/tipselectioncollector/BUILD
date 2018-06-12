cc_library(
    name = "shared",
    srcs = glob(
        ["**/*.cpp"],
        exclude = [
            "tipselectioncollector.cpp",
            "tests/**/*.cpp",
        ],
    ),
    hdrs = glob(
        ["**/*.hpp"],
        exclude = ["tests/**/*.hpp"],
    ),
    include_prefix = "iota/tanglescope/tipselectioncollector",
    deps = [
        "//tanglescope/common",
        "//tanglescope/prometheus_collector",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)

cc_library(
    name = "tipselectioncollector",
    srcs = ["tipselectioncollector.cpp"],
    hdrs = glob(["tipselectioncollector.hpp"]),
    include_prefix = "iota/tanglescope",
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        "@prometheus_cpp",
    ],
)
