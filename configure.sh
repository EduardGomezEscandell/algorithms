#!/usr/bin/bash

clear

# Options
export CC="clang"
export CXX="clang++"
export BUILD_TYPE=${BUILD_TYPE:-Release}
export IS_TEST_BUILD=${IS_TEST_BUILD:-false} # Indicates whether this build is made for testing purposes

# Path setup
export PROJECT_DIR="$(pwd -P)"
export BUILD_DIR="${PROJECT_DIR}/build"
export BIN_DIR="${PROJECT_DIR}/bin/${BUILD_TYPE}"
export SOURCE_DIR="${PROJECT_DIR}"

# Building
cmake                              \
-B"${BUILD_DIR}/${BUILD_TYPE}"     \
-DCMAKE_BUILD_TYPE=${BUILD_TYPE}   \
-DIS_TEST_BUILD=${IS_TEST_BUILD}   \
-DBIN_DIR="${BIN_DIR}"

if [ $? != 0 ]; then
    echo "Configure step failed"
    exit 1
else
    echo "Configure step successful"
fi

# Compile commands
ln -sf "${BUILD_DIR}/${BUILD_TYPE}/compile_commands.json" "${PROJECT_DIR}/compile_commands.json"

cmake --build "${BUILD_DIR}/${BUILD_TYPE}" -- -j$(nproc)

if [ $? != 0 ]; then
    echo "Build step failed"
    exit 1
else
    echo "Build step successful"
fi
