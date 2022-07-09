cmake_minimum_required(VERSION 3.16)

project(algorithms LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Werror)


add_subdirectory(external_libraries)
add_subdirectory(include)
add_subdirectory(demos)
add_subdirectory(tests)