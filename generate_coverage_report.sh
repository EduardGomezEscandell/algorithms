# Settings
export COVERAGE_DIR="$(pwd -P)/coverage"
export LLVM_PROFILE_FILE="${COVERAGE_DIR}/testcov.profraw"
export LLVM_PROFMERGE_FILE="${COVERAGE_DIR}/testcov.profdata"
export EXECUTABLE="./bin/Debug/run_tests"

# Dependencies
llvm-profdata --help > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Please install llvm:"
    echo "sudo apt install llvm"
    exit
fi

# Generate coverage report
mkdir -p ${COVERAGE_DIR}
${EXECUTABLE}

llvm-profdata merge -sparse ${LLVM_PROFILE_FILE} -o ${LLVM_PROFMERGE_FILE}

# Summary
llvm-cov                                                           \
report                                                             \
${EXECUTABLE}                                                      \
-instr-profile=${LLVM_PROFMERGE_FILE}                              \
-ignore-filename-regex="external_libraries/|tests/|demos/"         \

# Detailed report
llvm-cov show                                                      \
${EXECUTABLE}                                                      \
-instr-profile=${LLVM_PROFMERGE_FILE}                              \
-ignore-filename-regex="external_libraries/|tests/|demos/"         \
-format="html"                                                     \
-Xdemangler="c++filt"                                              \
-region-coverage-lt=100                                            \
> "${COVERAGE_DIR}/report.html"

# Cleanup
rm -rf ${LLVM_PROFILE_FILE} ${LLVM_PROFMERGE_FILE}

# Open report
( open "${COVERAGE_DIR}/report.html" &> /dev/null )