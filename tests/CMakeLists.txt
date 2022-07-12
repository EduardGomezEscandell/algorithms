add_executable(run_tests main.cpp)
target_link_libraries(run_tests doctest mylib)

add_custom_command(TARGET run_tests POST_BUILD
    COMMAND run_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Testing..."
)