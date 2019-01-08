def _queue_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.source_template,
        output = ctx.outputs.source,
        substitutions = {"{TYPE}": str(ctx.attr.type)},
    )
    ctx.actions.expand_template(
        template = ctx.file.header_template,
        output = ctx.outputs.header,
        substitutions = {"{TYPE}": str(ctx.attr.type)},
    )

_queue_generator = rule(
    implementation = _queue_generator_impl,
    attrs = {
        "source": attr.string(mandatory = True),
        "source_template": attr.label(mandatory = True, allow_single_file = True),
        "header": attr.string(mandatory = True),
        "header_template": attr.label(mandatory = True, allow_single_file = True),
        "type": attr.string(mandatory = True),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def queue_generate(type):
    base = str("lf_queue_") + str(type)
    source = base + ".c"
    header = base + ".h"

    _queue_generator(
        name = base + "_generator",
        source = source,
        source_template = "//utils/containers/lock_free:lf_queue.c.tpl",
        header = header,
        header_template = "//utils/containers/lock_free:lf_queue.h.tpl",
        type = type,
    )

    native.cc_library(
        name = base,
        srcs = [source],
        hdrs = [header],
        deps = [
            "//common:errors",
            "@liblfds",
            "//utils/containers:person_example",
            "//utils/containers/lock_free:lf_queue_struct",
        ],
        visibility = ["//visibility:public"],
    )
