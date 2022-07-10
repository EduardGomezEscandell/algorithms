#!/usr/bin/bash

clear

export CC="clang"
export CXX="clang++"

export PROJECT_DIR="$(pwd -P)"

export BUILD_TYPE=${BUILD_TYPE:-Release}
echo "Build type: ${BUILD_TYPE}"

export BUILD_DIR="${PROJECT_DIR}/build"
export BIN_DIR="${PROJECT_DIR}/bin/${BUILD_TYPE}"
export SOURCE_DIR="${PROJECT_DIR}"
# export DO_TEST=1

# Building
cmake                                               \
-B"${BUILD_DIR}/${BUILD_TYPE}" -H"${SOURCE_DIR}"    \
-DCMAKE_BUILD_TYPE=${BUILD_TYPE}                    \
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

if [ "${DO_TEST}" ]; then
    ./bin/${BUILD_TYPE}/run_tests
fi