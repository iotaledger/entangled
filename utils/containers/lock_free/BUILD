load(":container_generator.bzl", "container_generate")

container_generate(
    type = "int",
)

container_generate(
    include = "gossip/iota_packet.h",
    type = "iota_packet_t",
    deps = ["//gossip:iota_packet"],
)
