cc_library(
    name = "shared",
    hdrs = ["http.h"],
    visibility = ["//visibility:public"],
    deps = ["//cclient:service"],
)

cc_library(
    name = "beast",
    srcs = [
        "beast.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        "//common/model:transaction",
        "@boost//:beast",
        "@com_github_google_glog//:glog",
    ],
)
