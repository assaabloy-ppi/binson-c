#The minimum CMake version required to build this project
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

#Set a new CMake project
project(example C)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(COMMON_C_FLAGS  "-Wall -g -std=c99 -pedantic -pedantic-errors -Wextra -Winvalid-pch -Winit-self -Wno-unknown-pragmas")
  set(COMMON_C_FLAGS_DEBUG  "-Wall -g  -std=c99 -pedantic -pedantic-errors -Wextra -Winvalid-pch -Winit-self -Wno-unknown-pragmas")
  set(COMMON_C_FLAGS_RELWITHDEBINFO  "-Wall -g -std=c99  -pedantic -pedantic-errors -Wextra -Winvalid-pch -Winit-self -Wno-unknown-pragmas")
  set(COMMON_C_FLAGS_RELEASE  "-Wall -g -std=c99 -pedantic -pedantic-errors -Wextra -Winvalid-pch -Winit-self -Wno-unknown-pragmas")

  if(NOT WIN32)
    #set(COMMON_C_FLAGS "${COMMON_C_FLAGS} -Werror")
  endif()  
  set(CMAKE_C_FLAGS "${COMMON_C_FLAGS}" )  # replace default
  set(CMAKE_C_FLAGS_DEBUG "${COMMON_C_FLAGS_DEBUG}")  # replace default
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "${COMMON_C_FLAGS_RELWITHDEBINFO}")  # replace default
  set(CMAKE_C_FLAGS_RELEASE "${COMMON_C_FLAGS_RELEASE}")    # replace default
endif()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} "../src"  )

FILE( GLOB APP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c* )

foreach( testsourcefile ${APP_SOURCES} )   
    string( REPLACE ".c" "" testname ${testsourcefile} )
    if (NOT(${testname} STREQUAL "common"))
       add_executable( ${testname} ${testsourcefile}  common.c )    
       target_link_libraries( ${testname} binson )      
    endif(NOT(${testname} STREQUAL "common")) 
endforeach( testsourcefile ${APP_SOURCES} )
