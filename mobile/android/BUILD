cc_library(
    name = "jni",
    srcs = [
        "Interface.cpp",
        "jni.h",
    ],
    hdrs = ["Interface.h"],
    linkopts = [
        "-lm",
        "-ldl",
    ],
    deps = [
        "//common/helpers",
        "//utils:bundle_miner",
        "//utils:memset_safe",
    ],
    alwayslink = True,
)

android_binary(
    name = "dummy",
    srcs = [
        "demo/App.java",
        "demo/DummyActivity.java",
        "java/Interface.java",
    ],
    custom_package = "org.iota.mobile",
    manifest = "AndroidManifest.xml",
    deps = [":jni"],
)
