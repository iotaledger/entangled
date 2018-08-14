cc_library(
    name = "iota_lib_cpp",
    srcs = glob(["source/**/*.cpp"]),
    hdrs = glob(["include/**/*.hpp"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "@boost//:asio",
        "@boost//:beast",
        "@keccak",
        "@nlohmann//:json",
    ],
)
