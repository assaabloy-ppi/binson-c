project(binson-utest C)

function (ADD_CMOCKA_TEST _testName _testSource)
    add_executable(${_testName} ${_testSource})
    add_dependencies(${_testName} ${ARGN})
    target_link_libraries(${_testName} ${ARGN} )
    add_test(${_testName} ${CMAKE_CURRENT_BINARY_DIR}/${_testName})
endfunction (ADD_CMOCKA_TEST)

# add tests
add_cmocka_test(utest_util utest_util.c  binson btest cmocka_lib )
add_cmocka_test(utest_utf8 utest_utf8.c  binson btest cmocka_lib )
add_cmocka_test(utest_error utest_error.c  binson btest cmocka_lib  )
add_cmocka_test(utest_writer utest_writer.c  binson btest cmocka_lib )
add_cmocka_test(utest_token_buf utest_token_buf.c  binson btest cmocka_lib )
