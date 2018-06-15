cc_test(name="test_mam",
        srcs=["test_mam.c"],
        linkopts = ["-lpthread"],
        deps=
        [
          "//common/mam/v1:mam",
          "//common/trinary:trit_tryte",
          "@unity//:unity",
        ])

cc_test(name="test_mask",
        srcs=["test_mask.c"],
        deps=
        [
          "//common/mam/v1:mask",
          "//common/trinary:trit_tryte",
          "@unity//:unity",
        ])

cc_test(name="test_merkle",
        srcs=["test_merkle.c"],
        deps=
        [
          "//common/mam/v1:merkle",
          "//common/trinary:trit_tryte",
          "@unity//:unity",
        ])
