genrule(
    name = "snapshot_files",
    outs = [
        "snapshotMainnet.txt",
        "snapshotMainnet.sig",
        "snapshotTestnet.txt",
        "snapshotTestnet.sig",
    ],
    cmd =
        """
        url="https://raw.githubusercontent.com/iotaledger/iri/master/src/main/resources"
        wget -q $$url/snapshotMainnet.txt -O $(location snapshotMainnet.txt) && dos2unix $(location snapshotMainnet.txt)
        wget -q $$url/snapshotMainnet.sig -O $(location snapshotMainnet.sig) && dos2unix $(location snapshotMainnet.sig)
        wget -q $$url/snapshotTestnet.txt -O $(location snapshotTestnet.txt) && dos2unix $(location snapshotTestnet.txt)
        wget -q $$url/snapshotTestnet.sig -O $(location snapshotTestnet.sig) && dos2unix $(location snapshotTestnet.sig)
      """,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "snapshot",
    srcs = ["snapshot.c"],
    hdrs = ["snapshot.h"],
    data = [":snapshot_files"],
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
