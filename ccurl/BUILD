genrule(
    name = "opencl_xxd",
    srcs = [
        "src/lib/cl/pearl.cl",
    ],
    outs = ["src/lib/pearl.cl.h"],
    cmd = "xxd -i $(<) > $(@)",
)

cc_library(
    name = "opencl-stub",
    srcs = [
        "deps/libopencl-stub/src/libopencl.c",
    ],
    hdrs = glob(["deps/libopencl-stub/include/**/*.h"]),
    linkopts = ["-ldl"],
    strip_include_prefix = "deps/libopencl-stub/include",
)

cc_library(
    name = "ccurl",
    srcs = glob([
        "src/lib/*.c",
        "src/lib/**/*.h",
        "src/lib/**/*.c",
    ]),
    hdrs = [":opencl_xxd"] + glob(["src/lib/**/*.h"]),
    include_prefix = "ccurl",
    strip_include_prefix = "src/lib",
    visibility = ["//visibility:public"],
    deps = [
        ":opencl-stub",
    ],
)
