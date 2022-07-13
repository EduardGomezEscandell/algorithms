cmake_minimum_required(VERSION 3.16)

project(algorithms LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

message("Build type: ${CMAKE_BUILD_TYPE}")

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if(DO_INSTRUMENT)
    message("Instrumentation enabled")
    
    # Sanitizers
    add_compile_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak)
    add_link_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak)

    # Code coverage instrumentation
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    
else()
    message("Instrumentation disabled")
endif()

if (DO_BUILD_TESTS)
    message("Tests will be built")
    add_subdirectory(tests)
endif()

add_subdirectory(include)
add_subdirectory(external_libraries)
add_subdirectory(demos)

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
    )