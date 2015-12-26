# Binson test library
project(btest C)

set(BTEST_LIBRARY btest)


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

include_directories(
  ${CMOCKA_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include  
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# find cmocka library and headers
find_path(CMOCKA_ROOT_DIR
    NAMES include/cmocka.h )  
find_path(CMOCKA_INCLUDE_DIR
    NAMES cmocka.h
    PATHS ${CMOCKA_ROOT_DIR}/include)
find_library(CMOCKA_LIBRARY NAMES cmocka
    PATHS ${CMOCKA_ROOT_DIR}/lib )

set(BINSON_TEST_LINK_LIBRARIES
    ${CMOCKA_LIBRARY}
    )
    
    
# create test library
add_library(btest STATIC btest.c)
target_link_libraries(btest ${CMOCKA_LIBRARY} )


add_subdirectory(utest)
