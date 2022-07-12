cmake_minimum_required(VERSION 3.16)

project(algorithms LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

message("Build type: ${CMAKE_BUILD_TYPE}")

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if(IS_TEST_BUILD)
    message("This build is for testing.")
    
    # Sanitizers
    add_compile_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak)
    add_link_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak)

    # Code coverage instrumentation
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    
    add_subdirectory(tests)
else()
    message("This build is not for testing.")
endif()

add_subdirectory(include)
add_subdirectory(external_libraries)
add_subdirectory(demos)
