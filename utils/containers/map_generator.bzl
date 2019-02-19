def _map_generator_impl(ctx):
    ctx.actions.expand_template(
        template = ctx.file.source_template,
        output = ctx.outputs.source,
        substitutions = {"{KEY_TYPE}": ctx.attr.key_type, "{VALUE_TYPE}": ctx.attr.value_type, "{PARENT_DIRECTORY}": ctx.attr.parent_directory},
    )

    ADDITIONAL_INCLUDE_PATH_ACTUAL = ""
    if ctx.attr.additional_include_path != "":
        ADDITIONAL_INCLUDE_PATH_ACTUAL = "#include " + "\"" + ctx.attr.additional_include_path + "\""

    ctx.actions.expand_template(
        template = ctx.file.header_template,
        output = ctx.outputs.header,
        substitutions = {"{KEY_TYPE}": ctx.attr.key_type, "{VALUE_TYPE}": ctx.attr.value_type, "{ADDITIONAL_INCLUDE_PATH}": ADDITIONAL_INCLUDE_PATH_ACTUAL},
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
        "additional_include_path": attr.string(mandatory = False),
        "additional_deps": attr.string_list(mandatory = False),
        "parent_directory": attr.string(mandatory = False),
    },
    outputs = {
        "source": "%{source}",
        "header": "%{header}",
    },
)

def map_generate(key_type, value_type, additional_include_path = "", additional_deps = [], parent_directory = "utils/containers"):
    base = key_type + "_to_" + value_type + "_map"
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
            "//common/trinary:trits",
            "@com_github_uthash//:uthash",
        ] + additional_deps,
        visibility = ["//visibility:public"],
    )
