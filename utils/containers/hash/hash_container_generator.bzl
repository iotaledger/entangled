def _hash_container_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.source_template,
        output = ctx.outputs.source,
        substitutions = {"{SIZE}": str(ctx.attr.size)},
    )
    ctx.actions.expand_template(
        template = ctx.file.header_template,
        output = ctx.outputs.header,
        substitutions = {"{SIZE}": str(ctx.attr.size)},
    )

_hash_container_generator = rule(
    implementation = _hash_container_generator_impl,
    attrs = {
        "source": attr.string(mandatory = True),
        "source_template": attr.label(mandatory = True, allow_single_file = True),
        "header": attr.string(mandatory = True),
        "header_template": attr.label(mandatory = True, allow_single_file = True),
        "size": attr.int(mandatory = True),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def hash_container_generate(type, size):
    base = "hash" + str(size) + "_" + type
    source = base + ".c"
    header = base + ".h"

    _hash_container_generator(
        name = base + "_generator",
        source = source,
        source_template = "//utils/containers/hash:hash_" + type + ".c.tpl",
        header = header,
        header_template = "//utils/containers/hash:hash_" + type + ".h.tpl",
        size = size,
    )

    native.cc_library(
        name = base,
        srcs = [source],
        hdrs = [header],
        deps = [
            "//common:errors",
            "//common/trinary:flex_trit",
            "//utils/handles:rand",
            "@com_github_uthash//:uthash",
        ],
        visibility = ["//visibility:public"],
    )
