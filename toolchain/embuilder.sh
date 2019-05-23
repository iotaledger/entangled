#!/bin/bash
set -euo pipefail
EM_CONFIG="LLVM_ROOT='$PWD/external/emscripten_clang';"
#EM_CONFIG+="EMSCRIPTEN_NATIVE_OPTIMIZER='$PWD/external/emscripten_clang/optimizer';"
EM_CONFIG+="BINARYEN_ROOT='$PWD/external/emscripten_clang/binaryen';"
EM_CONFIG+="NODE_JS='$PWD/external/nodejs/node/bin/node';"
EM_CONFIG+="EMSCRIPTEN_ROOT='$PWD/external/emscripten_toolchain';"
EM_CONFIG+="SPIDERMONKEY_ENGINE='';"
EM_CONFIG+="V8_ENGINE='';"
# EM_CONFIG+="TEMP_DIR='$PWD/tmp';"
EM_CONFIG+="COMPILER_ENGINE=NODE_JS;"
EM_CONFIG+="JS_ENGINES=[NODE_JS];"
export EM_CONFIG

export EM_EXCLUSIVE_CACHE_ACCESS=1
export EMCC_SKIP_SANITY_CHECK=1
# export EMCC_DEBUG=1
mkdir -p "/tmp/emscripten_cache"
export EM_CACHE="/tmp/emscripten_cache"
# export EMCC_TEMP_DIR="$PWD/tmp"
export EMCC_WASM_BACKEND=0
python external/emscripten_toolchain/embuilder.py "$@"

# Remove the first line of .d file (emscripten resisted all my attempts to make
# it realize it's just the absolute location of the source)
find . -name "*.d" -exec sed -i '' '2d' {} \;