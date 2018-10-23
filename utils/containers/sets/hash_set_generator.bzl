def hash_set_generate(size):
    native.genrule(
        name = "hash" + str(size) + "_set_generate_header",
        outs = ["hash" + str(size) + "_set.h"],
        srcs = ["hash_set_template.h"],
        cmd = "sed 's/$$SIZE/" + str(size) + "/g' $(<) > $(@)",
    )

    native.genrule(
        name = "hash" + str(size) + "_set_generate_source",
        outs = ["hash" + str(size) + "_set.c"],
        srcs = ["hash_set_template.c"],
        cmd = "sed 's/$$SIZE/" + str(size) + "/g' $(<) > $(@)",
    )

    native.cc_library(
        name = "hash" + str(size) + "_set",
        srcs = ["hash" + str(size) + "_set.c"],
        hdrs = ["hash" + str(size) + "_set.h"],
        deps = [
            "//common:errors",
            "//common/trinary:flex_trit",
            "@com_github_uthash//:uthash",
        ],
    )
