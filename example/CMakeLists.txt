#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

#Set a new CMake project
project(example C)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} "../src"  )

FILE( GLOB APP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c* )

foreach( testsourcefile ${APP_SOURCES} )   
    string( REPLACE ".c" "" testname ${testsourcefile} )
    if (NOT(${testname} STREQUAL "common"))
       add_executable( ${testname} ${testsourcefile}  common.c )    
       target_link_libraries( ${testname} binson )      
    endif(NOT(${testname} STREQUAL "common")) 
endforeach( testsourcefile ${APP_SOURCES} )
