cc_library(
    name="jni",
    hdrs=["Interface.h"],
    srcs=["Interface.cc"],
    deps=["//mobile/interface:interface"])

android_binary(
    name="hello_android",
    custom_package="org.iota.mobile",
    srcs=["java/Interface.java", "demo/App.java", "demo/DummyActivity.java"],
    manifest="AndroidManifest.xml",
    deps=[":jni", "//mobile/interface:interface"])
