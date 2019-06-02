cc_library(
    name = "serializer_json",
    srcs = glob([
        "json/*.c",
    ]),
    hdrs = glob([
        "json/*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":serializer",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_queue",
        "//utils/containers/hash:hash243_stack",
        "//utils/containers/hash:hash8019_queue",
        "//utils/containers/hash:hash8019_stack",
        "//utils/containers/hash:hash81_queue",
        "//utils/containers/hash:hash_array",
        "@cJSON",
        "@com_github_uthash//:uthash",
    ],
    alwayslink = True,
)

cc_library(
    name = "serializer",
    hdrs = glob([
        "serializer.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//cclient/request:requests",
        "//cclient/response:responses",
    ],
)
