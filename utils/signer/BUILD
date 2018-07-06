cc_binary(
    name = "mssign",
    srcs = ["mssign.c"],
    data = [],
    deps = 
        [
          "@com_github_gflags_gflags//:gflags",
          "//common/sign/v2:iss_curl",
          "//common/curl-p:trit",
          "//common/kerl:kerl",
          "//common/kerl:hash",
          "//common/trinary:trit_tryte",
          "//mam/v1:merkle",
        ],
)

cc_binary(
    name = "msstree",
    srcs = ["msstree.c"],
    data = [],
    deps = 
        [
          "@com_github_gflags_gflags//:gflags",
          "//common/sign/v2:iss_curl",
          "//common/curl-p:trit",
          "//common/kerl:kerl",
          "//common/trinary:trit_tryte",
          "//mam/v1:merkle",
        ],
)
