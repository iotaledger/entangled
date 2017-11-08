workspace(name="org_iota_entangled")

#load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

http_archive(
    name="com_github_gflags_gflags",
    url="https://github.com/gflags/gflags/archive/v2.2.1.tar.gz",
    strip_prefix="gflags-2.2.1",
    sha256="ae27cdbcd6a2f935baa78e4f21f675649271634c092b1be01469440495609d0e")

new_http_archive(
    name="com_github_google_googletest",
    url="https://github.com/google/googletest/archive/release-1.8.0.tar.gz",
    strip_prefix="googletest-release-1.8.0",
    sha256="58a6f4277ca2bc8565222b3bbd58a177609e9c488e8a72649359ba51450db7d8",
    build_file="external/BUILD.gtest")

new_http_archive(
    name="com_github_dled_influxdbcpprest",
    url=
    "https://github.com/d-led/influxdb-cpp-rest/archive/50f597b2fbfa361dede850d4d54288f1c40c6f74.zip",
    strip_prefix="influxdb-cpp-rest-50f597b2fbfa361dede850d4d54288f1c40c6f74",
    sha256="a6f3508d24ead6c92e54f87d117a73ee056e971d08d0ead9a1846c1d3247d250",
    build_file="external/BUILD.influxdb-cpp-rest")

new_http_archive(
    name="com_github_google_glog",
    url="https://github.com/google/glog/archive/v0.3.5.tar.gz",
    sha256="7580e408a2c0b5a89ca214739978ce6ff480b5e7d8d7698a2aa92fadc484d1e0",
    strip_prefix="glog-0.3.5",
    build_file="external/BUILD.glog", )

new_http_archive(
    name="com_github_reactiveextensions_rxcpp",
    url="https://github.com/Reactive-Extensions/RxCpp/archive/v4.0.0.tar.gz",
    sha256="50e7395ab1bc2a0000df126c6920a36dd3c4ee04a71496b2f4c10adf50d65178",
    strip_prefix="RxCpp-4.0.0",
    build_file="external/BUILD.rxcpp", )

new_http_archive(
    name="com_github_jedisct1_libsodium",
    url="https://github.com/jedisct1/libsodium/archive/1.0.15.tar.gz",
    strip_prefix="libsodium-1.0.15",
    sha256="8889835aee49f3e216bb0f25d1b15aefd0b84da6dcdedbbcff1f77d881083407",
    build_file="external/BUILD.libsodium")

new_http_archive(
    name="com_github_zeromq_libzmq",
    url="https://github.com/zeromq/libzmq/archive/v4.2.2.tar.gz",
    strip_prefix="libzmq-4.2.2",
    sha256="ea5eecbb9cc231a750535a7b74f887b18656014cc92601e8ab34734db799b796",
    build_file="external/BUILD.libzmq")

new_http_archive(
    name="com_github_zeromq_cppzmq",
    url="https://github.com/zeromq/cppzmq/archive/v4.2.2.tar.gz",
    strip_prefix="cppzmq-4.2.2",
    sha256="3ef50070ac5877c06c6bb25091028465020e181bbfd08f110294ed6bc419737d",
    build_file="external/BUILD.cppzmq")

new_http_archive(
    name="com_github_fmtlib_fmt",
    url="https://github.com/fmtlib/fmt/archive/4.0.0.tar.gz",
    strip_prefix="fmt-4.0.0",
    sha256="35300a0d356529447a79ed5ccf419239d8b34f916e5d4625f046fd37afa3650a",
    build_file="external/BUILD.fmt")

new_http_archive(
    name="com_github_microsoft_cpprestsdk",
    url="https://github.com/Microsoft/cpprestsdk/archive/v2.10.0.tar.gz",
    strip_prefix="cpprestsdk-2.10.0",
    sha256="de333da67f1cb3d1b30be118860531092467f18d24ca6b4d36f6623fecab0de0",
    build_file="external/BUILD.cpprestsdk")

http_archive(
    name="com_github_nelhage_boost",
    url=
    "https://github.com/nelhage/rules_boost/archive/8865ee01e1b903fe63bc6615a0e25d86ee9855d5.zip",
    strip_prefix="rules_boost-8865ee01e1b903fe63bc6615a0e25d86ee9855d5",
    sha256="5c08516754a09129eac046ddd767d2e896ef9f63c625d1e1f00685a9ebf9c5ea",
)

load("@com_github_nelhage_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

bind(
    name="influxdb_cpp_rest",
    actual="@com_github_dled_influxdbcpprest//:influxdb_cpp_rest")
bind(
    name="cpprestsdk",
    actual="@com_github_microsoft_cpprestsdk//:cpprestsdk", )
bind(
    name="fmt",
    actual="@com_github_fmtlib_fmt//:fmt", )
bind(
    name="cppzmq",
    actual="@com_github_zeromq_cppzmq//:cppzmq", )
bind(
    name="zmq",
    actual="@com_github_zeromq_libzmq//:zmq", )
bind(
    name="sodium",
    actual="@com_github_jedisct1_libsodium//:sodium", )
bind(
    name="rxcpp",
    actual="@com_github_reactiveextensions_rxcpp//:rxcpp", )
bind(
    name="glog",
    actual="@com_github_google_glog//:glog", )
bind(
    name="gtest_main",
    actual="@com_github_google_googletest//:gtest_main", )
bind(
    name="gtest",
    actual="@com_github_google_googletest//:gtest", )
bind(
    name="gflags",
    actual="@com_github_gflags_gflags//:gflags", )
