def _container_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.source_template,
        output = ctx.outputs.source,
        substitutions = {"{TYPE}": str(ctx.attr.type)},
    )
    ctx.actions.expand_template(
        template = ctx.file.header_template,
        output = ctx.outputs.header,
        substitutions = {
          "{TYPE}": str(ctx.attr.type),
          "{INCLUDE}": ("#include \"" + str(ctx.attr.include) + "\"") if str(ctx.attr.include) else ""},
    )

_container_generator = rule(
    implementation = _container_generator_impl,
    attrs = {
        "source": attr.string(mandatory = True),
        "source_template": attr.label(mandatory = True, allow_single_file = True),
        "header": attr.string(mandatory = True),
        "header_template": attr.label(mandatory = True, allow_single_file = True),
        "type": attr.string(mandatory = True),
        "include": attr.string(mandatory = False),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def container_generate(type, include=None, deps=[]):
    base = str("lf_mpmc_queue_") + str(type)
    source = base + ".c"
    header = base + ".h"

    _container_generator(
        name = base + "_generator",
        source = source,
        source_template = "//utils/containers/lock_free:lf_mpmc_queue.c.tpl",
        header = header,
        header_template = "//utils/containers/lock_free:lf_mpmc_queue.h.tpl",
        type = type,
        include = include,
    )

    native.cc_library(
        name = base,
        srcs = [source],
        hdrs = [header],
        deps = [
            "//common:errors",
            "@concurrencykit",
        ] + deps,
        visibility = ["//visibility:public"],
    )
