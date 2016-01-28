#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

# Binson fuzz test suite
project(binson_fuzzer Java)

find_package(Java REQUIRED)
include(UseJava)

set(CMAKE_JAVA_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

add_jar(binson_fuzzer SOURCES FuzzGen.java binson_fuzzer.java
                      INCLUDE_JARS lib/binson-dev.jar  lib/commons-lang3-3.4.jar
		      ENTRY_POINT binson_fuzzer
		      OUTPUT_DIR "${CMAKE_BINARY_DIR}/test/fuzz_test"
		      )

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/"  DESTINATION "${CMAKE_BINARY_DIR}/test/fuzz_test" )
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/binson_fuzzer.sh"  DESTINATION "${CMAKE_BINARY_DIR}/test/fuzz_test" )

