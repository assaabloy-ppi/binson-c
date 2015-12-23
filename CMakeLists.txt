#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#Set a new CMake project
project(binson C)

# The version number.
set (BINSON_MAJOR_VERSION 0)
set (BINSON_MINOR_VERSION 0)
set (BINSON_MICRO_VERSION 2)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/binson_config.h.in"
  "${PROJECT_BINARY_DIR}/binson_config.h"
  )
  
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")  

file(GLOB SOURCES "src/*.c")


add_library(binson SHARED ${SOURCES})

target_include_directories(binson PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)


add_subdirectory(example)
