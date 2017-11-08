genrule(
    name='opencl_xxd',
    srcs=[
        'src/lib/cl/pearl.cl',
    ],
    outs=['src/lib/pearl.cl.h'],
    cmd="xxd -i $(<) > $(@)", )

cc_library(
    name='opencl-stub',
    hdrs=glob(['deps/libopencl-stub/include/**/*.h']),
    strip_include_prefix='deps/libopencl-stub/include',
    linkopts=['-ldl'],
    srcs=[
        'deps/libopencl-stub/src/libopencl.c',
    ])

cc_library(
    name='ccurl',
    deps=[
        ':opencl-stub',
    ],
    hdrs=[':opencl_xxd'] + glob(['src/lib/**/*.h']),
    srcs=glob([
        'src/lib/*.c',
        'src/lib/**/*.h',
        'src/lib/**/*.c',
    ]),
    include_prefix='ccurl',
    strip_include_prefix='src/lib',
    visibility=['//visibility:public'])
