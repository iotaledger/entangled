genrule(
    name = "snapshot_mainnet",
    outs = ["snapshotMainnet.txt"],
    cmd = "wget -q https://raw.githubusercontent.com/iotaledger/iri/dev/src/main/resources/$$(basename $@) -O $@",
)

genrule(
    name = "snapshot_mainnet_sig",
    outs = ["snapshotMainnet.sig"],
    cmd = "wget -q https://raw.githubusercontent.com/iotaledger/iri/dev/src/main/resources/$$(basename $@) -O $@",
)

genrule(
    name = "snapshot_testnet",
    outs = ["snapshotTestnet.txt"],
    cmd = "wget -q https://raw.githubusercontent.com/iotaledger/iri/dev/src/main/resources/$$(basename $@) -O $@",
)

cc_library(
    name = "snapshot",
    srcs = ["snapshot.c"],
    hdrs = ["snapshot.h"],
    data = [
        ":snapshot_mainnet",
        ":snapshot_mainnet_sig",
        ":snapshot_testnet",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//consensus:defs",
        "//consensus/snapshot:state_delta",
        "//utils:logger_helper",
        "//utils:signed_files",
        "//utils/handles:rw_lock",
    ],
)

cc_library(
    name = "state_delta",
    srcs = ["state_delta.c"],
    hdrs = ["state_delta.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils:hash_containers",
    ],
)
