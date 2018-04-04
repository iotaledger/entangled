package(default_visibility=["//visibility:public"])

cc_library(name='iss_curl',
           srcs=[
               'iss_curl.c',
               'iss.c.inc',
           ],
           hdrs=[
               'iss_curl.h',
               'iss.h.inc',
           ],
           deps=[
               '//common/curl-p:trit',
               '//common/trinary:add',
           ], )

cc_library(name='iss_kerl',
           srcs=[
               'iss_kerl.c',
               'iss.c.inc',
           ],
           hdrs=[
               'iss_kerl.h',
               'iss.h.inc',
           ],
           deps=[
               '//common/kerl:kerl',
               '//common/trinary:add',
           ], )
