#!/bin/bash

set -e

BAZEL_VERSION=0.7.0
BAZEL_INSTALLER=${PWD}/bazel-installer/install.sh
INSTALLER_PLATFORM=linux-x86_64
BASE="${PWD}/bazel-install"

# Fetch the Bazel installer
rm -rf ${BASE}
rm -rf ${PWD}/bazel-installer
mkdir ${BASE}
mkdir ${PWD}/bazel-installer

URL=https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/bazel-${BAZEL_VERSION}-installer-${INSTALLER_PLATFORM}.sh
curl -L -o ${BAZEL_INSTALLER} ${URL}

# Install bazel inside ${BASE}
bash "${BAZEL_INSTALLER}" \
     --base="${BASE}" \
     --bin="${BASE}/binary"

export CC=/usr/bin/gcc-7
export CXX=/usr/bin/g++-7

# Run the build
BAZEL="${BASE}/binary/bazel --bazelrc=${PWD}/tools/bazel.rc"
${BAZEL} test //... --verbose_failures
