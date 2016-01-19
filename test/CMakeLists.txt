# Binson test library
project(btest C)

set(BTEST_LIBRARY btest)


if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(COMMON_C_FLAGS  "-Wall -g -std=c99  -fsanitize=address -fstack-protector-all -pedantic -pedantic-errors -Wextra -Winvalid-pch -Winit-self -Wno-unknown-pragmas")
  set(COMMON_C_FLAGS_DEBUG  "-Wall -g  -std=c99  -fsanitize=address -fstack-protector-all -pedantic -pedantic-errors -Wextra -Winvalid-pch -Winit-self -Wno-unknown-pragmas")
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

include(ExternalProject)
ExternalProject_Add (
        cmocka
        GIT_REPOSITORY "git://git.cryptomilk.org/projects/cmocka.git"
        GIT_TAG "master"
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        DOWNLOAD_DIR ""
        CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/cmocka"
        INSTALL_DIR ${CMAKE_BINARY_DIR}/external/cmocka
        INSTALL_COMMAND
          "cmake"
          --build .
          --target install
        )
        
add_library(cmocka_lib SHARED IMPORTED)       
set_target_properties( cmocka_lib PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/external/cmocka/lib/libcmocka.so" )
add_dependencies( cmocka_lib cmocka )

# find cmocka library and headers
#find_path(CMOCKA_ROOT_DIR
#    PATHS "${CMAKE_BINARY_DIR}/external/cmocka"
#    NAMES include/cmocka.h )  
#find_path(CMOCKA_INCLUDE_DIR
#    NAMES cmocka.h
#    PATHS ${CMOCKA_ROOT_DIR}/include)
#find_library(CMOCKA_LIBRARY NAMES cmocka
#    PATHS ${CMOCKA_ROOT_DIR}/lib )

set(CMOCKA_INCLUDE_DIR  "${CMAKE_BINARY_DIR}/external/cmocka/include")

include_directories(
  ${CMOCKA_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include  
  ${CMAKE_CURRENT_SOURCE_DIR}
)
    
# create test library
add_library(btest STATIC btest.c)
add_dependencies(btest cmocka_lib)
target_link_libraries(btest cmocka_lib )
#target_link_libraries(btest cmocka)

add_subdirectory(utest)