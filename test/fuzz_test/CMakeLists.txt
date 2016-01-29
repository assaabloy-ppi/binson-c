#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

# Binson fuzz test suite
project(fuzz_test C)

#set(CMAKE_C_FLAGS  "-g -O0 -pedantic -pedantic-errors -Wall -W  -Wsign-conversion -Wshadow -Wpointer-arith -Wcast-align  -Wnested-externs -fshort-enums -fno-common -Dinline=  ")

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include  
)

add_subdirectory(binson_fuzzer)
    
add_executable(fuzz_test fuzz_test.c)
target_link_libraries(fuzz_test  binson)      

add_custom_target(fuzz
    #COMMAND java -jar ./binson_fuzzer.jar 2 2 100000
    #COMMAND java -cp ./binson_fuzzer/binson_fuzzer.jar binson_fuzzer 2 2 100000 JSON

    #Usage: binson_fuzzer.jar <f> <l> <x> [JSON]
    #	<f>: Integer in range 0..100 to specify starting fuzz factor
    #	<l>: Integer in range 0..100 to specify ending fuzz factor
    #	<x>: Integer to specify number of fuzz iterations before fuzz factor increment
    #	[JSON]: Generate JSON output instead of Binson
    COMMAND java -cp "binson_fuzzer.jar:*"  binson_fuzzer 3 10 10000
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/fuzz_test
    DEPENDS fuzz_test
)

add_dependencies(fuzz_test binson_fuzzer)
add_dependencies(fuzz fuzz_test binson_fuzzer)