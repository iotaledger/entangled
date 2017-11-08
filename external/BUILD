cc_library(
    name='pthread',
    exported_linker_flags=['-lpthread', '-ldl'],
    visibility=['PUBLIC'], )

cc_library(
    name='opencl',
    exported_linker_flags=['-lOpenCL'],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_system',
    exported_linker_flags=['-lboost_system'],
    deps=[
        ':pthread',
    ],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_filesystem',
    exported_linker_flags=['-lboost_filesystem'],
    deps=[
        ':boost_system',
    ],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_context',
    exported_linker_flags=['-lboost_context'],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_chrono',
    exported_linker_flags=['-lboost_chrono'],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_atomic',
    exported_linker_flags=['-lboost_atomic'],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_thread',
    exported_linker_flags=['-lboost_thread'],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_random',
    exported_linker_flags=['-lboost_random'],
    visibility=['PUBLIC'], )

cc_library(
    name='boost_regex',
    exported_linker_flags=['-lboost_regex'],
    visibility=['PUBLIC'], )

cc_library(
    name='openssl_ssl',
    exported_linker_flags=['-lssl'],
    deps=[
        ':openssl_crypto',
    ],
    visibility=['PUBLIC'], )

cc_library(
    name='openssl_crypto',
    exported_linker_flags=['-lcrypto'],
    deps=[
        ':pthread',
    ],
    visibility=['PUBLIC'], )
