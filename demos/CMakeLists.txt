add_executable(mergesort mergesort.cpp)
target_include_directories(mergesort INTERFACE ..)
target_link_libraries(mergesort mylib)

add_executable(karatsuba karatsuba.cpp)
target_include_directories(karatsuba INTERFACE ..)
target_link_libraries(karatsuba mylib)
