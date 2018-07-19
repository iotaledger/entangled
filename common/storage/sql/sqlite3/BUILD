cc_library(
    name = "sqlite3_storage",
    srcs = [
        "connection.cc",
        "storage.cc",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage",
        "//common/storage/sql:connection",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@sqlite3",
    ],
)
