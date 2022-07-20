add_executable(mergesort mergesort.cpp)
target_include_directories(mergesort INTERFACE ..)
target_link_libraries(mergesort mylib)

add_executable(karatsuba karatsuba.cpp)
target_include_directories(karatsuba INTERFACE ..)
target_link_libraries(karatsuba mylib)

add_executable(count_inversions count_inversions.cpp)
target_include_directories(count_inversions INTERFACE ..)
target_link_libraries(count_inversions mylib)

add_executable(count_comparisons count_comparisons.cpp)
target_include_directories(count_comparisons INTERFACE ..)
target_link_libraries(count_comparisons mylib)
