def _hash_map_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.source_template,
        output = ctx.outputs.source,
        substitutions = {"{TYPE}": ctx.attr.mapped_type},
    )
    ctx.actions.expand_template(
        template = ctx.file.header_template,
        output = ctx.outputs.header,
        substitutions = {"{TYPE}": ctx.attr.mapped_type},
    )

_hash_map_generator = rule(
    implementation = _hash_map_generator_impl,
    attrs = {
        "source": attr.string(mandatory = True),
        "source_template": attr.label(mandatory = True, allow_single_file = True),
        "header": attr.string(mandatory = True),
        "header_template": attr.label(mandatory = True, allow_single_file = True),
        "mapped_type": attr.string(mandatory = True),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def hash_map_generate(mapped_type):
    base = "hash_" + mapped_type + "_map"
    source = base + ".c"
    header = base + ".h"

    _hash_map_generator(
        name = base + "_generator",
        source = source,
        source_template = "//utils/containers/hash:hash_map.c.tpl",
        header = header,
        header_template = "//utils/containers/hash:hash_map.h.tpl",
        mapped_type = mapped_type,
    )

    native.cc_library(
        name = base,
        srcs = [source],
        hdrs = [header],
        deps = [
            "//common:errors",
            "//common/trinary:flex_trit",
            "//utils/handles:rand",
            "//utils/containers/hash:hash243_set",
            "@com_github_uthash//:uthash",
        ],
        visibility = ["//visibility:public"],
    )
