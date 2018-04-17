cc_library(
    name="jni",
    hdrs=["Interface.h"],
    srcs=["Interface.cpp", "jni.h"],
    deps=['//common/helpers:helpers'])

android_binary(
    name="dummy",
    custom_package="org.iota.mobile",
    srcs=["java/Interface.java", "demo/App.java", "demo/DummyActivity.java"],
    manifest="AndroidManifest.xml",
    deps=[":jni"])
