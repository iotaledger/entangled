## IOTA Entangled Monorepo
[![Build status](https://badge.buildkite.com/42ef0512276a270bd34cb0010fd641558b344dab4aa2225fba.svg)](https://buildkite.com/iota-foundation/entangled)

This is a monorepo containing all you need to get [[IOTA]](https://iota.org/) operating in C/C++

Containing IOTA components, models as well as cryptography primitives used in IOTA.  

# Content:

Component|Description
--- | ---
[[Tanglescope]](https://github.com/iotaledger/entangled/tree/develop/tanglescope) | A monitoring tool for the Tangle
[[MAM]](https://github.com/iotaledger/MAM) |A low level implementation for MAM
[[CIRI]](https://github.com/iotaledger/entangled/tree/develop/ciri) (WIP) |A low level implementation of IOTA node largely inspired by [[IRI]](https://github.com/iotaledger/iri)
[[CClient]](https://github.com/iotaledger/entangled/tree/develop/cclient) | A low level IOTA client implementation


## Building

* Please use [Bazel](https://www.bazel.build/):
```shell
bazel test //...
```

## Developing Entangled
- Be sure to run `./tools/hooks/autohook.sh install` after initial checkout!
- Pass `-c dbg` for building with debug symbols.

### Tools required for running git commit hook
- buildifier
- clang-format

### Buildifier
Buildifier can be installed with `bazel` or `go`

#### Install with go
1. change directory to `$GOPATH`
2. run `$ go get github.com/bazelbuild/buildtools/buildifier`
   The executable file will be located under `$GOPATH/bin`
3. make a soft link for global usage, run
   `$ sudo ln -s $HOME/go/bin/buildifier /usr/bin/buildifier`

#### Install with bazel
1. clone `bazelbuild/buildtools` repository
   `$ git clone https://github.com/bazelbuild/buildtools.git`
2. change directory to `buildtools`
3. build it with bazel command, `$ bazel build //buildifier`
   The executable file will be located under `path/to/buildtools/bazel-bin`
4. make a soft link

### clang-format
clang-format can be installed by command:
- Debian/Ubuntu based: `$ sudo apt-get install clang-format`
- OSX: `$ brew install clang-format`

# Contributors

| [<img src="https://avatars1.githubusercontent.com/u/3305068?v=4" width="60px;" alt="thibault-martinez"/><br /><sub><b>thibault-martinez</b></sub>](https://github.com/thibault-martinez)  | [<img src="https://avatars1.githubusercontent.com/u/37177579?v=4" width="60px;" alt="tsvisabo"/><br /><sub><b>tsvisabo</b></sub>](https://github.com/tsvisabo) | [<img src="https://avatars1.githubusercontent.com/u/30996?v=4" width="60px;" alt="th0br0"/><br /><sub><b>th0br0</b></sub>](https://github.com/th0br0)  | [<img src="https://avatars1.githubusercontent.com/u/462383?v=4" width="60px;" alt="oopsmonk"/><br /><sub><b>oopsmonk</b></sub>](https://github.com/oopsmonk)  | [<img src="https://avatars3.githubusercontent.com/u/3903636?v=4" width="60px;" alt="ifullgaz"/><br /><sub><b>ifullgaz</b></sub>](https://github.com/ifullgaz)  |
|:---:|:---:|:---:|:---:|:---:|
| [<img src="https://avatars3.githubusercontent.com/u/13924801?v=4" width="60px;" alt="HowJMay"/><br /><sub><b>HowJMay</b></sub>](https://github.com/HowJMay)  | [<img src="https://avatars3.githubusercontent.com/u/11289354?v=4" width="60px;" alt="jkrvivian"/><br /><sub><b>jkrvivian</b></sub>](https://github.com/jkrvivian)  | [<img src="https://avatars3.githubusercontent.com/u/19519564?v=4" width="60px;" alt="rajivshah3"/><br /><sub><b>rajivshah3</b></sub>](https://github.com/rajivshah3)  |   |   |
