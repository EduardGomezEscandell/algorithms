# Algorithms
This repository contains the code I write as I follow Stanford's Algorithms Specialization on Coursera. This code is not homework, just diferent topics that the course proposes or touches upon.

### How to build
```bash
export BUILD_TYPE=Release         # This line is optional
bash configure.sh
```
You can choose CMake's typical Release, RelWithDebugInfo, Debug, FullDebug

### How to run
After compiling with a certain build type, you can run one of the demos by calling:
```
./bin/${BUILD_TYPE}/${DEMO}
```

### Testing
After compiling with a certain build type, you can run one of the demos by calling:
```
./bin/${BUILD_TYPE}/run_tests
```
