# IOTA Entangled Monorepo
[![Build status](https://badge.buildkite.com/42ef0512276a270bd34cb0010fd641558b344dab4aa2225fba.svg)](https://buildkite.com/iota-foundation/entangled)

## Building

* Please use [Bazel](https://www.bazel.build/):
```shell
bazel test //...
```

## Developing Entangled 
- Be sure to run `./hooks/autohook.sh install` after initial checkout!
- Pass `-c dbg` for building with debug symbols.