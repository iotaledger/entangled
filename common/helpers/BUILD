cc_library(name="helpers",
           deps=[":digest",
                 ":sign",
                 ":pow"],
           visibility=["//visibility:public"])

cc_library(name="sign",
          deps=["//common/trinary:trit_tryte",
                "//common/kerl:kerl",
                "//common/sign/v1:iss_kerl",
                ":pow"],
          srcs=["sign.c"],
          hdrs=["sign.h"],
          visibility=["//visibility:public"])

cc_library(name="pow",
           deps=["//common/pow:pow"],
           srcs=["pow.c"],
           hdrs=["pow.h"],
           visibility=["//visibility:public"])

cc_library(name="digest",
          deps=["//common/curl-p:digest"],
          srcs=["digest.c"],
          hdrs=["digest.h"],
          visibility=["//visibility:public"])

cc_test(name='tests',
      srcs=glob([
         'tests/**/*.cpp',
         '**/*.hpp',
      ]),
      deps=[
         '@gtest//:gtest_main',
         '@gtest//:gtest',
         ':helpers',
      ],
      tags=["exclusive"],
      timeout = "short")