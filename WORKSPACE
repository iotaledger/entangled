workspace(name="org_iota_entangled")

load(":repositories.bzl", "entangled_cpp_repositories")

entangled_cpp_repositories()

http_archive(
    name="com_github_nelhage_boost",
    url=
    "https://github.com/nelhage/rules_boost/archive/8865ee01e1b903fe63bc6615a0e25d86ee9855d5.zip",
    strip_prefix="rules_boost-8865ee01e1b903fe63bc6615a0e25d86ee9855d5",
    sha256="5c08516754a09129eac046ddd767d2e896ef9f63c625d1e1f00685a9ebf9c5ea",
)

load("@com_github_nelhage_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

