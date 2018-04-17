package(default_visibility = ["//visibility:public"])

cc_library (
    name = "curl-p-const",
    hdrs = [
        "const.h",
        "indices.h",
        ]
    )

cc_library (
    name = "trit",
    srcs = ["curl_p.c"],
    hdrs = [
        "trit.h",
        "indices.h",
        ],
    deps = [
        "//common:stdint",
        "//common/trinary:trits",
        ":curl-p-const",
        ],
    )

cc_library (
    name = "bct",
    srcs = ["curl_p_bct.c"],
    hdrs = [
        "bct.h",
        ],
    deps = [
        "//common:stdint",
        "//common/trinary:trits",
        "//common/trinary:bct",
        ":curl-p-const",
        ],
    )

cc_library (
    name = "ptrit",
    srcs = ["curl_p_ptrit.c"],
    hdrs = [
        "ptrit.h",
        ],
    deps = [
        "//common:stdint",
        "//common/trinary:trits",
        "//common/trinary:ptrits",
        ":curl-p-const",
        ],
    )

cc_library (
    name = "search",
    hdrs = [
        "search.h",
        ],
    deps = [
        "//common:stdint",
        "//common/trinary:trits",
        "//common/trinary:trit_ptrit",
        "//common/trinary:ptrits",
        ":ptrit",
        ":trit",
        ":curl-p-const",
        ],
    )

cc_library (
    name = "pearl_diver",
    srcs = [
        "pearl_diver.c",
        ],
    hdrs = [
        "pearl_diver.h",
        ],
    deps = [
    "//common:stdint",
        "//common/trinary:trits",
        "//common/trinary:trit_ptrit",
        "//common/trinary:ptrits",
        ":ptrit",
        ":trit",
        ":search",
        ":curl-p-const",
        ],
    )

cc_library (
    name = "hashcash",
    srcs = [
        "hashcash.c",
        ],
    hdrs = [
        "hashcash.h",
        ],
    deps = [
        ":pearl_diver",
        ":curl-p-const",
        ],
    )

cc_library (
    name = "hamming",
    srcs = [
        "hamming.c",
        ],
    hdrs = [
        "hamming.h",
        ],
    deps = [
        ":pearl_diver",
        ":curl-p-const",
        ],
    )

cc_library(name="digest",
      deps=[":trit",
            "//common/trinary:trit_tryte"
            ],
      srcs=["digest.c"],
      hdrs=["digest.h"],
      visibility=["//visibility:public"]
      )
