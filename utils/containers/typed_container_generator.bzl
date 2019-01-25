def _typed_container_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.source_template,
        output = ctx.outputs.source,
        substitutions = {"{TYPE}": ctx.attr.value_type, "{PARENT_DIRECTORY}": ctx.attr.parent_directory},
    )

    ADDITIONAL_INCLUDE_PATH_ACTUAL = ""
    if ctx.attr.additional_include_path != "":
        ADDITIONAL_INCLUDE_PATH_ACTUAL = "#include " + "\"" + ctx.attr.additional_include_path + "\""

    ctx.actions.expand_template(
        template = ctx.file.header_template,
        output = ctx.outputs.header,
        substitutions = {"{TYPE}": ctx.attr.value_type, "{ADDITIONAL_INCLUDE_PATH}": ADDITIONAL_INCLUDE_PATH_ACTUAL},
    )

_type_container_generator = rule(
    implementation = _typed_container_generator_impl,
    attrs = {
        "source": attr.string(mandatory = True),
        "source_template": attr.label(mandatory = True, allow_single_file = True),
        "header": attr.string(mandatory = True),
        "header_template": attr.label(mandatory = True, allow_single_file = True),
        "container_type": attr.string(mandatory = True),
        "value_type": attr.string(mandatory = True),
        "additional_include_path": attr.string(mandatory = False),
        "additional_deps": attr.string(mandatory = False),
        "parent_directory": attr.string(mandatory = False),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def typed_container_generate(container_type, value_type, additional_include_path = "", additional_deps = "//utils/containers:dummy_dep", parent_directory = "utils/containers"):
    base = value_type + "_" + container_type
    source = base + ".c"
    header = base + ".h"

    _type_container_generator(
        name = base + "_generator",
        source = source,
        source_template = "//utils/containers:" + container_type + ".c.tpl",
        header = header,
        header_template = "//utils/containers:" + container_type + ".h.tpl",
        container_type = container_type,
        value_type = value_type,
        additional_include_path = additional_include_path,
        additional_deps = additional_deps,
        parent_directory = parent_directory,
    )

    native.cc_library(
        name = base,
        srcs = [source],
        hdrs = [header],
        deps = [
            "//common:errors",
            "@com_github_uthash//:uthash",
            additional_deps,
        ],
        visibility = ["//visibility:public"],
    )
