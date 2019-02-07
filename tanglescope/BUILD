cc_library(
    name = "shared",
    srcs = glob(
        ["**/*.cpp"],
        exclude = [
            "tipselectioncollector.cpp",
            "tanglewidthcollector.cpp",
            "blowballcollector.cpp",
            "echocollector.cpp",
            "confirmationratecollector.cpp",
            "broadcastrecievecollecter.cpp",
            "tests/**/*.cpp",
        ],
    ),
    hdrs = glob(
        ["**/*.hpp"],
        exclude = ["tests/**/*.hpp"],
    ),
    deps = [
        "//tanglescope/common",
        "//tanglescope/prometheus_collector",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@prometheus_cpp",
    ],
)

cc_library(
    name = "tipselectioncollector",
    srcs = ["tipselectioncollector.cpp"],
    hdrs = ["tipselectioncollector.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
    ],
)

cc_library(
    name = "tanglewidthcollector",
    srcs = ["tanglewidthcollector.cpp"],
    hdrs = ["tanglewidthcollector.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        "//common/helpers",
        "//common/trinary:tryte_long",
    ],
)

cc_library(
    name = "blowballcollector",
    srcs = ["blowballcollector.cpp"],
    hdrs = ["blowballcollector.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
    ],
)

cc_library(
    name = "echocollector",
    srcs = ["echocollector.cpp"],
    hdrs = ["echocollector.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        ":throwcatchcollector",
    ],
)

cc_library(
    name = "throwcatchcollector",
    srcs = ["broadcastrecievecollecter.cpp"],
    hdrs = ["broadcastrecievecollecter.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
    ],
)

cc_library(
    name = "confirmationratecollector",
    srcs = ["confirmationratecollector.cpp"],
    hdrs = ["confirmationratecollector.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        ":throwcatchcollector",
    ],
)
