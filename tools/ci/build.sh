#!/bin/bash

set -e

BAZEL_VERSION=0.11.1
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

export CC=/usr/bin/gcc
export CXX=/usr/bin/g++

# Run the build
BAZEL="${BASE}/binary/bazel --bazelrc=${PWD}/tools/bazel.rc"

if [ $1 -eq "64" ]; then
	echo "================================================================================="
	echo "================================================================================="
	echo "================================================================================="
	echo "==================================64 BIT TESTS==================================="
	echo "================================================================================="
	echo "================================================================================="
	echo "================================================================================="
	${BAZEL} test //... --verbose_failures --deleted_packages="//mobile/ios"
elif [ $1 -eq "32" ]; then
	echo "================================================================================="
	echo "================================================================================="
	echo "================================================================================="
	echo "==================================32 BIT TESTS==================================="
	echo "================================================================================="
	echo "================================================================================="
	echo "================================================================================="

	# FIXME @th0br0 these build opts are also passed to Android clang causing build failure for aarch64
	${BAZEL} test --linkopt="-m32" --copt="-m32" --cxxopt="-m32" --deleted_packages="//mobile/android, //mobile/ios" //... --verbose_failures
else 
	exit 1
fi
