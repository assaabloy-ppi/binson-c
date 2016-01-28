/*
 *	Unit tests for 'binson_utf8' module
 */
#include <string.h>

#include "btest.h"

#include "binson_utf8.h"


static void utest_binson_utf8_is_valid(void **state) {
    (void) state;    
    /*see ref: http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt */  
    
    /*************** must be VALID *************/
    assert_true(binson_utf8_is_valid( (uint8_t*)"κόσμε"));
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5")); // "κόσμε"
    
    assert_true(binson_utf8_is_valid( (uint8_t*)"\x00"));	 // 2.1.1
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xC2\x80")); 	// 2.1.2
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xE0\xA0\x80")); // 2.1.3
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xF0\x90\x80\x80")); // 2.1.4    

    assert_true(binson_utf8_is_valid( (uint8_t*)"\x7F"));	 // 2.2.1
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xDF\xBF"));	 // 2.2.2
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xEF\xBF\xBF"));	 // 2.2.3
    
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xED\x9F\xBF"));	 // 2.3.1
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xEE\x80\x80"));	 // 2.3.2
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xEF\xBF\xBD"));	 // 2.3.3
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xF4\x8F\xBF\xBF"));	 // 2.3.4
            
    /* According to Unicode Corrigendum #9, noncharacter usage is now legal even in open data interchange */
    /* Ref: http://www.unicode.org/versions/corrigendum9.html */
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xef\xbf\xbe")); // 5.3.1
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xef\xbf\xbf")); // 5.3.2
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xEF\xB7\x90\xEF\xB7\x91\xEF\xB7\x92\xEF\xB7\x93\xEF\xB7\x94\xEF\xB7\x95\xEF\xB7\x96"
						 "\xEF\xB7\x97\xEF\xB7\x98\xEF\xB7\x99\xEF\xB7\x9A\xEF\xB7\x9B\xEF\xB7\x9C\xEF\xB7\x9D"
						 "\xEF\xB7\x9E\xEF\xB7\x9F\xEF\xB7\xA0\xEF\xB7\xA1\xEF\xB7\xA2\xEF\xB7\xA3\xEF\xB7\xA4"
						 "\xEF\xB7\xA5\xEF\xB7\xA6\xEF\xB7\xA7\xEF\xB7\xA8\xEF\xB7\xA9\xEF\xB7\xAA\xEF\xB7\xAB"
						 "\xEF\xB7\xAC\xEF\xB7\xAD\xEF\xB7\xAE\xEF\xB7\xAF")); // 5.3.3
    
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xF0\x9F\xBF\xBE\xF0\x9F\xBF\xBF\xF0\xAF\xBF\xBE\xF0\xAF\xBF\xBF\xF0\xBF\xBF\xBE\xF0"
						 "\xBF\xBF\xBF\xF1\x8F\xBF\xBE\xF1\x8F\xBF\xBF\xF1\x9F\xBF\xBE\xF1\x9F\xBF\xBF\xF1\xAF"
						 "\xBF\xBE\xF1\xAF\xBF\xBF\xF1\xBF\xBF\xBE\xF1\xBF\xBF\xBF\xF2\x8F\xBF\xBE\xF2\x8F\xBF\xBF"						 
						 "\xF2\x9F\xBF\xBE\xF2\x9F\xBF\xBF\xF2\xAF\xBF\xBE\xF2\xAF\xBF\xBF\xF2\xBF\xBF\xBE\xF2\xBF"
						 "\xBF\xBF\xF3\x8F\xBF\xBE\xF3\x8F\xBF\xBF\xF3\x9F\xBF\xBE\xF3\x9F\xBF\xBF\xF3\xAF\xBF\xBE"
						 "\xF3\xAF\xBF\xBF\xF3\xBF\xBF\xBE\xF3\xBF\xBF\xBF\xF4\x8F\xBF\xBE\xF4\x8F\xBF\xBF")); // 5.3.4    
    
    /* planes 4-15. see: https://www.w3.org/International/questions/qa-forms-utf-8 */
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xf1\xbe\x80\x9d"));
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xef\x82\xac\xe8\x87\xbd"));
    
    assert_true(binson_utf8_is_valid( (uint8_t*)"\x1c\xe9\xa1\x99"));    
    assert_true(binson_utf8_is_valid( (uint8_t*)"\xe7\x88\x85\xec\x9b\xa1"));

    assert_true(binson_utf8_is_valid( (uint8_t*)"\xf3\x91\xb0\xa7"));   
    
    
    /*************** must be INVALID *************/
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xBF")); // 3.1.2.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\x80\xBF")); // 3.1.3.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\x80\xBF\x80")); // 3.1.4.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\x80\xBF\x80\xBF")); // 3.1.5.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\x80\xBF\x80\xBF\x80")); // 3.1.6.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\x80\xBF\x80\xBF\x80\xBF")); // 3.1.7.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\x80\xBF\x80\xBF\x80\xBF\x80")); // 3.1.8.
   
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F"
						 "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F"
						 "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF"
						 "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF")); // 3.1.9.

    assert_true(!binson_utf8_is_valid( (uint8_t*)
	"\xC0\x20\xC1\x20\xC2\x20\xC3\x20\xC4\x20\xC5\x20\xC6\x20\xC7\x20\xC8\x20\xC9\x20\xCA\x20\xCB\x20\xCC\x20\xCD\x20\xCE\x20\xCF\x20"
	"\xD0\x20\xD1\x20\xD2\x20\xD3\x20\xD4\x20\xD5\x20\xD6\x20\xD7\x20\xD8\x20\xD9\x20\xDA\x20\xDB\x20\xDC\x20\xDD\x20\xDE\x20\xDF\x20")); // 3.2.1.

    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xE0\x20\xE1\x20\xE2\x20\xE3\x20\xE4\x20\xE5\x20\xE6\x20\xE7\x20\xE8\x20\xE9\x20\xEA\x20\xEB\x20\xEC\x20\xED\x20\xEE\x20\xEF\x20")); // 3.2.2.

    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF0\x20\xF1\x20\xF2\x20\xF3\x20\xF4\x20\xF5\x20\xF6\x20\xF7\x20")); // 3.2.3.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF8\x20\xF9\x20\xFA\x20\xFB\x20")); // 3.2.4.
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xFC\x20\xFD\x20")); // 3.2.5.
    
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xC0")); // 3.3.1
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xE0\x80")); // 3.3.2
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF0\x80\x80")); // 3.3.3
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF8\x80\x80\x80")); // 3.3.4
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xFC\x80\x80\x80\x80")); // 3.3.5
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xDF")); // 3.3.6
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xEF\xBF")); // 3.3.7
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF7\xBF\xBF")); // 3.3.8
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xFB\xBF\xBF\xBF")); // 3.3.9
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xFD\xBF\xBF\xBF\xBF")); // 3.3.10
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xC0\xE0\x80\xF0\x80\x80\xF8\x80\x80\x80\xFC\x80\x80\x80\x80\xDF\xEF\xBF\xF7\xBF\xBF\xFB\xBF\xBF\xBF\xFD\xBF\xBF\xBF\xBF")); // 3.4
    
    assert_true(!binson_utf8_is_valid( (uint8_t*)" \xfe ")); // 3.5.1
    assert_true(!binson_utf8_is_valid( (uint8_t*)" \xff ")); // 3.5.2  
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xfe\xfe\xff\xff")); // 3.5.3
    
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xc0\xaf")); // 4.1.1
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xe0\x80\xaf")); // 4.1.2
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xf0\x80\x80\xaf")); // 4.1.3
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xf8\x80\x80\x80\xaf")); // 4.1.4
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xfc\x80\x80\x80\x80\xaf")); // 4.1.5
    
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xc1\xbf")); // 4.2.1
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xe0\x9f\xbf")); // 4.2.2
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xf0\x8f\xbf\xbf")); // 4.2.3
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xf8\x87\xbf\xbf\xbf")); // 4.2.4
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xfc\x83\xbf\xbf\xbf\xbf")); // 4.2.5
    
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xc0\x80")); // 4.3.1
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xe0\x80\x80")); // 4.3.2
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xf0\x80\x80\x80")); // 4.3.3
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xf8\x80\x80\x80\x80")); // 4.3.4
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xfc\x80\x80\x80\x80\x80")); // 4.3.5
    
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xa0\x80")); // 5.1.1
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xad\xbf")); // 5.1.2
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xae\x80")); // 5.1.3
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xaf\xbf")); // 5.1.4
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xb0\x80")); // 5.1.5
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xbe\x80")); // 5.1.6
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xbf\xbf")); // 5.1.7
                                              
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xa0\x80\xed\xb0\x80")); // 5.2.1
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xa0\x80\xed\xbf\xbf")); // 5.2.2
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xad\xbf\xed\xb0\x80")); // 5.2.3
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xad\xbf\xed\xbf\xbf")); // 5.2.4
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xae\x80\xed\xb0\x80")); // 5.2.5
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xae\x80\xed\xbf\xbf")); // 5.2.6
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xaf\xbf\xed\xb0\x80")); // 5.2.7
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xed\xaf\xbf\xed\xbf\xbf")); // 5.2.8
                          
    /* We limit utf-8 codepoint by U+10FFFF */
    /* UTF-8 was originally specified to allow codepoints with up to 31 bits (or 6 bytes). 
    But with RFC3629, this was reduced to 4 bytes max */
    
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF8\x88\x80\x80\x80")); // 2.1.5
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xFC\x84\x80\x80\x80\x80")); // 2.1.6

    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF7\xBF\xBF\xBF"));	 // 2.2.4
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xFB\xBF\xBF\xBF\xBF"));	 // 2.2.5
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xFD\xBF\xBF\xBF\xBF\xBF"));	 // 2.2.6
    assert_true(!binson_utf8_is_valid( (uint8_t*)"\xF4\x90\x80\x80"));	 // 2.3.5    
}


static void utest_binson_utf8_unescape(void **state) {
    (void)	state;    
    size_t	res_size = 0;
    uint8_t	buf[256];
    
#define UTEST_UTF8_UNESCAPE(u,h)  \
    res_size = binson_utf8_unescape(buf, sizeof(buf), (uint8_t*)u); \
    assert_int_equal( strcmp(h, (char*)buf), 0 ); \
    assert_int_equal( res_size, sizeof(h)-1 ); 

    UTEST_UTF8_UNESCAPE("\\u0024", "\x24")
    UTEST_UTF8_UNESCAPE("\\u00A2", "\xC2\xA2")
    UTEST_UTF8_UNESCAPE("\\u20AC", "\xE2\x82\xAC")
    UTEST_UTF8_UNESCAPE("\\U00010348", "\xF0\x90\x8D\x88") // 𐍈  (U+10348 is in supplementary plane, plane 1 of UTF-8)

    UTEST_UTF8_UNESCAPE("\\u03BA\\u1F79\\u03C3\\u03BC\\u03B5", "\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5")  //κόσμε
    UTEST_UTF8_UNESCAPE("\\u0000", "\x00")
    UTEST_UTF8_UNESCAPE("\\u0080", "\xC2\x80")
    UTEST_UTF8_UNESCAPE("\\u0800", "\xE0\xA0\x80")
    UTEST_UTF8_UNESCAPE("\\U00010000", "\xF0\x90\x80\x80")
    UTEST_UTF8_UNESCAPE("\\u007F", "\x7F")
    UTEST_UTF8_UNESCAPE("\\u07FF", "\xDF\xBF")
    UTEST_UTF8_UNESCAPE("\\uD7FF", "\xED\x9F\xBF")
    UTEST_UTF8_UNESCAPE("\\uE000", "\xEE\x80\x80")
    UTEST_UTF8_UNESCAPE("\\uFFFD", "\xEF\xBF\xBD")
    UTEST_UTF8_UNESCAPE("\\U0010FFFF", "\xF4\x8F\xBF\xBF")
    UTEST_UTF8_UNESCAPE("\\uFFFE", "\xef\xbf\xbe")
    UTEST_UTF8_UNESCAPE("\\uFFFF", "\xef\xbf\xbf")
    UTEST_UTF8_UNESCAPE("gr\\u00F6\\u00DFer", "gr\xC3\xB6\xC3\x9F""er")  // größer
    UTEST_UTF8_UNESCAPE("\\u6DD8\\u5B9D\\u7F51", "\xE6\xB7\x98\xE5\xAE\x9D\xE7\xBD\x91") // 淘宝网
}


int utest_run_tests(void) {
  const struct CMUnitTest tests[] = {
	      cmocka_unit_test(utest_binson_utf8_is_valid),
	      cmocka_unit_test(utest_binson_utf8_unescape),	      
	      };
  return cmocka_run_group_tests(tests, NULL, NULL);
}