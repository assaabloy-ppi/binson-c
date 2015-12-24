#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

#Set a new CMake project
project(binson C)

option (BUILD_LIB_STATIC "Build binson lib as static instead of dynamic" OFF) 
option (WITH_BINSON_JSON_OUTPUT "Build lib with JSON output support" ON) 


# The version number.
set (BINSON_MAJOR_VERSION 0)
set (BINSON_MINOR_VERSION 0)
set (BINSON_MICRO_VERSION 2)

# platform endiannes checks
include(TestBigEndian)
TEST_BIG_ENDIAN(BIGENDIAN)
if (NOT ${BIGENDIAN})
  set (IS_LITTLE_ENDIAN 1)
endif (NOT ${BIGENDIAN})

if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(DEBUG 1)
else()
    set(NDEBUG 1)
endif()

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

if (BUILD_LIB_STATIC)
  add_library(binson STATIC ${SOURCES})
else (BUILD_LIB_STATIC)
  add_library(binson SHARED ${SOURCES})
endif (BUILD_LIB_STATIC)

# target_include_directories is not supported for early versions
if (NOT("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" LESS 2.8.12))
  target_include_directories(binson PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)
else ()
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()

add_subdirectory(example)

########################################################################
## Build Summary

message(
    "\n====================|  Binson Build Summary  |====================="
    "\n"
    "\nBuild type:        ${CMAKE_BUILD_TYPE}"    
    "\nInstall prefix:    ${CMAKE_INSTALL_PREFIX}"
    "\n"
    "\nCC:                ${CMAKE_C_COMPILER}"
    "\nCFLAGS:            ${CMAKE_C_FLAGS}"
    "\n================================================================\n"
)

########################################################################
## Installation

install(TARGETS binson DESTINATION lib)

# install public interface headers
#install(DIRECTORY include/${PROJECT_NAME}/
#        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
include(GNUInstallDirs)
install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

#install(FILES .... DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )


########################################################################
## Testing



