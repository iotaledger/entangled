def _map_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.source_template,
        output = ctx.outputs.source,
        substitutions = {"{KEY_TYPE}": str(ctx.attr.key_type), "{VALUE_TYPE}": str(ctx.attr.value_type)},
    )
    ctx.actions.expand_template(
        template = ctx.file.header_template,
        output = ctx.outputs.header,
        substitutions = {"{KEY_TYPE}": str(ctx.attr.key_type), "{VALUE_TYPE}": str(ctx.attr.value_type)},
    )

_map_generator = rule(
    implementation = _map_generator_impl,
    attrs = {
        "source": attr.string(mandatory = True),
        "source_template": attr.label(mandatory = True, allow_single_file = True),
        "header": attr.string(mandatory = True),
        "header_template": attr.label(mandatory = True, allow_single_file = True),
        "key_type": attr.string(mandatory = True),
        "value_type": attr.string(mandatory = True),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def map_generate(key_type, value_type):
    base = str(key_type) + str("_to_") + str(value_type) + "_map"
    source = base + ".c"
    header = base + ".h"

    _map_generator(
        name = base + "_generator",
        source = source,
        source_template = "//utils/containers:map.c.tpl",
        header = header,
        header_template = "//utils/containers:map.h.tpl",
        key_type = key_type,
        value_type = value_type,
    )

    native.cc_library(
        name = base,
        srcs = [source],
        hdrs = [header],
        deps = [
            "//common:errors",
            "@com_github_uthash//:uthash",
            "//common/trinary:flex_trit",
            "//utils/containers:person_example",
            "//gossip:neighbor",
        ],
        visibility = ["//visibility:public"],
    )
