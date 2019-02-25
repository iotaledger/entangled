cc_library(
    name = "jni",
    srcs = [
        "Interface.cpp",
        "jni.h",
    ],
    hdrs = ["Interface.h"],
    defines = [
        "APP_PLATFORM=android-19",
        "ANDROID_NATIVE_API_LEVEL=19",
    ],
    linkopts = ["-lm"],
    deps = ["//common/helpers"],
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
