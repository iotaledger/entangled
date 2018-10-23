def _hash_set_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file._source_template,
        output = ctx.outputs.source,
        substitutions = {"{SIZE}": str(ctx.attr.size)},
    )
    ctx.actions.expand_template(
        template = ctx.file._header_template,
        output = ctx.outputs.header,
        substitutions = {"{SIZE}": str(ctx.attr.size)},
    )

_hash_set_generator = rule(
    implementation = _hash_set_generator_impl,
    attrs = {
        "size": attr.int(mandatory = True),
        "source": attr.string(mandatory = True),
        "header": attr.string(mandatory = True),
        "_source_template": attr.label(
            default = Label("//utils/containers/sets:hash_set.c.tpl"),
            allow_single_file = True,
        ),
        "_header_template": attr.label(
            default = Label("//utils/containers/sets:hash_set.h.tpl"),
            allow_single_file = True,
        ),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def hash_set_generate(size):
    base = "hash" + str(size) + "_set"
    source = base + ".c"
    header = base + ".h"

    _hash_set_generator(
        name = base + "_generator",
        source = source,
        header = header,
        size = size,
    )

    native.cc_library(
        name = base,
        srcs = [source],
        hdrs = [header],
        deps = [
            "//common:errors",
            "//common/trinary:flex_trit",
            "@com_github_uthash//:uthash",
        ],
        visibility = ["//visibility:public"],
    )
