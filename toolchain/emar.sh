#!/bin/bash
set -euo pipefail
EM_CONFIG="LLVM_ROOT='external/emscripten_clang';"
EM_CONFIG+="EMSCRIPTEN_NATIVE_OPTIMIZER='external/emscripten_clang/optimizer';"
EM_CONFIG+="BINARYEN_ROOT='external/emscripten_clang/binaryen';"
EM_CONFIG+="NODE_JS='external/nodejs/node/bin/node';"
EM_CONFIG+="EMSCRIPTEN_ROOT='external/emscripten_toolchain';"
EM_CONFIG+="SPIDERMONKEY_ENGINE='';"
EM_CONFIG+="V8_ENGINE='';"
EM_CONFIG+="TEMP_DIR='tmp';"
EM_CONFIG+="COMPILER_ENGINE=NODE_JS;"
EM_CONFIG+="JS_ENGINES=[NODE_JS];"
export EM_CONFIG

export EM_EXCLUSIVE_CACHE_ACCESS=1
export EMCC_SKIP_SANITY_CHECK=1
# export EMCC_DEBUG=1
export EMCC_WASM_BACKEND=0
export EMMAKEN_NO_SDK=1

python external/emscripten_toolchain/emar.py "$@"