#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

# Binson fuzz test suite
project(fuzz_test C)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include  
)
    
add_executable(fuzz_test fuzz_test.c)
target_link_libraries(fuzz_test  binson)      

add_custom_target(fuzz
    COMMAND java -jar ./binson_fuzzer.jar 0 10 10
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/fuzz_test
    DEPENDS fuzz_test
)
