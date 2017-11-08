def load_gflags():
    native.http_archive(
        name="com_github_gflags_gflags",
        url="https://github.com/gflags/gflags/archive/v2.2.1.tar.gz",
        strip_prefix="gflags-2.2.1",
        sha256=
        "ae27cdbcd6a2f935baa78e4f21f675649271634c092b1be01469440495609d0e")


def load_gtest():
    native.new_http_archive(
        name="gtest",
        url="https://github.com/google/googletest/archive/release-1.8.0.tar.gz",
        strip_prefix="googletest-release-1.8.0",
        sha256=
        "58a6f4277ca2bc8565222b3bbd58a177609e9c488e8a72649359ba51450db7d8",
        build_file="external/BUILD.gtest")


def load_influxdb_cpp_rest():
    native.new_http_archive(
        name="influxdb_cpp_rest",
        url=
        "https://github.com/d-led/influxdb-cpp-rest/archive/50f597b2fbfa361dede850d4d54288f1c40c6f74.zip",
        strip_prefix=
        "influxdb-cpp-rest-50f597b2fbfa361dede850d4d54288f1c40c6f74",
        sha256=
        "a6f3508d24ead6c92e54f87d117a73ee056e971d08d0ead9a1846c1d3247d250",
        build_file="external/BUILD.influxdb-cpp-rest")


def load_glog():
    native.new_http_archive(
        name="glog",
        url="https://github.com/google/glog/archive/v0.3.5.tar.gz",
        sha256=
        "7580e408a2c0b5a89ca214739978ce6ff480b5e7d8d7698a2aa92fadc484d1e0",
        strip_prefix="glog-0.3.5",
        build_file="external/BUILD.glog", )


def load_rxcpp():
    native.new_http_archive(
        name="rxcpp",
        url=
        "https://github.com/Reactive-Extensions/RxCpp/archive/v4.0.0.tar.gz",
        sha256=
        "50e7395ab1bc2a0000df126c6920a36dd3c4ee04a71496b2f4c10adf50d65178",
        strip_prefix="RxCpp-4.0.0",
        build_file="external/BUILD.rxcpp", )


def load_libsodium():
    native.new_http_archive(
        name="libsodium",
        url="https://github.com/jedisct1/libsodium/archive/1.0.15.tar.gz",
        strip_prefix="libsodium-1.0.15",
        sha256=
        "8889835aee49f3e216bb0f25d1b15aefd0b84da6dcdedbbcff1f77d881083407",
        build_file="external/BUILD.libsodium")


def load_libzmq():
    native.new_http_archive(
        name="libzmq",
        url="https://github.com/zeromq/libzmq/archive/v4.2.2.tar.gz",
        strip_prefix="libzmq-4.2.2",
        sha256=
        "ea5eecbb9cc231a750535a7b74f887b18656014cc92601e8ab34734db799b796",
        build_file="external/BUILD.libzmq")


def load_cppzmq():
    native.new_http_archive(
        name="cppzmq",
        url="https://github.com/zeromq/cppzmq/archive/v4.2.2.tar.gz",
        strip_prefix="cppzmq-4.2.2",
        sha256=
        "3ef50070ac5877c06c6bb25091028465020e181bbfd08f110294ed6bc419737d",
        build_file="external/BUILD.cppzmq")


def load_fmtlib():
    native.new_http_archive(
        name="fmtlib",
        url="https://github.com/fmtlib/fmt/archive/4.0.0.tar.gz",
        strip_prefix="fmt-4.0.0",
        sha256=
        "35300a0d356529447a79ed5ccf419239d8b34f916e5d4625f046fd37afa3650a",
        build_file="external/BUILD.fmt")


def load_cpprestsdk():
    native.new_http_archive(
        name="cpprestsdk",
        url="https://github.com/Microsoft/cpprestsdk/archive/v2.10.0.tar.gz",
        strip_prefix="cpprestsdk-2.10.0",
        sha256=
        "de333da67f1cb3d1b30be118860531092467f18d24ca6b4d36f6623fecab0de0",
        build_file="external/BUILD.cpprestsdk")


def entangled_cpp_repositories():
    load_cpprestsdk()
    load_fmtlib()
    load_cppzmq()
    load_libzmq()
    load_libsodium()
    load_rxcpp()
    load_glog()
    load_gtest()
    load_gflags()
    load_influxdb_cpp_rest()
