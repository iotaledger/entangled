package(default_visibility=["//visibility:public"])

cc_library(name="mam",
           srcs=["mam.c"],
           hdrs=["mam.h"],
           include_prefix="mam",
           deps=
           [
            ":mask",
            ":merkle",
            "//common/curl-p:hamming",
            "//common/trinary:trit_long",
           ]
)

cc_library(name="mask",
           srcs=["mask.c"],
           hdrs=["mask.h"],
           include_prefix="mam",
           deps=
           [
            "//common/curl-p:trit",
            "//common/trinary:add"
           ]
)

cc_library(name="merkle",
           srcs=["merkle.c"],
           hdrs=["merkle.h"],
           include_prefix="mam",
           deps=["//common/sign/v2:iss_curl"]
)
