#!/usr/bin/bash

clear

# Options
export CC="clang"
export CXX="clang++"
export BUILD_TYPE=${BUILD_TYPE:-Release}
export DO_BUILD_TESTS=${DO_BUILD_TESTS:-false} # Indicates whether to build tests
export DO_INSTRUMENT=${DO_INSTRUMENT:-false} # Indicates whether to add instrumentation

# Path setup
export PROJECT_DIR="$(pwd -P)"
export BUILD_DIR="${PROJECT_DIR}/build"
export BIN_DIR="${PROJECT_DIR}/bin/${BUILD_TYPE}"
export SOURCE_DIR="${PROJECT_DIR}"

# Building
cmake                               \
-B"${BUILD_DIR}/${BUILD_TYPE}"      \
-DCMAKE_BUILD_TYPE=${BUILD_TYPE}    \
-DDO_BUILD_TESTS=${DO_BUILD_TESTS}  \
-DDO_INSTRUMENT=${DO_INSTRUMENT}    \
-DBIN_DIR="${BIN_DIR}"

if [ $? != 0 ]; then
    echo "Configure step failed"
    exit 1
else
    echo "Configure step successful"
fi

cmake --build "${BUILD_DIR}/${BUILD_TYPE}" -- -j$(nproc)

if [ $? != 0 ]; then
    echo "Build step failed"
    exit 1
else
    echo "Build step successful"
fi
