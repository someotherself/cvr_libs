#define CVRS_IMPLEMENTATION
#include "./../cvr_string.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

void assert_fixed_eq()
{
    cvr_fixed_str base_str;
    cvr_fixed_str lowercaps_str;

    base_str = cvr_fixed_from_cstr("Hello WORLD!");
    lowercaps_str = cvr_fixed_from_cstr("hello world!");

    assert(!cvr_fixed_eq(&base_str, &lowercaps_str));
    assert(!cvr_fixed_eq_cstr(&base_str, "Hello world!"));

    assert(cvr_fixed_eq_ignore_case(&base_str, &lowercaps_str));
    assert(cvr_fixed_eq_ignore_case_cstr(&base_str, "Hello world!"));

    assert(cvr_fixed_eq_cstr(&base_str, "Hello WORLD!"));
}

void assert_fixed_trim_whitespace()
{
    cvr_fixed_str base_str;
    cvr_fixed_str start_trim_str;
    cvr_fixed_str end_trim_str;

    cvr_fixed_str start_trim_check_str;
    cvr_fixed_str end_trim_check_str;

    base_str = cvr_fixed_from_cstr("     Hello world!       \n");
    start_trim_check_str = cvr_fixed_from_cstr("Hello world!       \n");
    end_trim_check_str = cvr_fixed_from_cstr("     Hello world!");
    
    start_trim_str = cvr_fixed_trim_whitespace_start(&base_str);
    end_trim_str = cvr_fixed_trim_whitespace_end(&base_str);

    assert(cvr_fixed_eq(&start_trim_str, &start_trim_check_str));
    assert(cvr_fixed_eq_cstr(&start_trim_str, "Hello world!       \n"));

    assert(cvr_fixed_eq(&end_trim_str, &end_trim_check_str));
    assert(cvr_fixed_eq_cstr(&end_trim_str, "     Hello world!"));
}

void assert_fixed_preffix_suffix_chop()
{
    cvr_fixed_str base_str;
    cvr_fixed_str start_chop_str;
    cvr_fixed_str end_chop_str;
    cvr_fixed_str start_end_chop_str;

    cvr_fixed_str start_chop_check_str;
    cvr_fixed_str end_chop_check_str;
    cvr_fixed_str start_end_chop_check_str;

    base_str = cvr_fixed_from_cstr("Hello world!");
    start_chop_check_str = cvr_fixed_from_cstr("Hello");
    end_chop_check_str = cvr_fixed_from_cstr("rld!");
    start_end_chop_check_str = cvr_fixed_from_cstr("lo wo");
    
    start_chop_str = cvr_fixed_take_until(&base_str, 5);
    end_chop_str = cvr_fixed_take_after(&base_str, 8);
    start_end_chop_str = cvr_fixed_slice_between(&base_str, 3, 8);

    assert(cvr_fixed_eq(&start_chop_str, &start_chop_check_str));
    assert(cvr_fixed_eq_cstr(&start_chop_str, "Hello"));

    assert(cvr_fixed_eq(&end_chop_str, &end_chop_check_str));
    assert(cvr_fixed_eq_cstr(&end_chop_str, "rld!"));

    assert(cvr_fixed_eq(&start_end_chop_str, &start_end_chop_check_str));
    assert(cvr_fixed_eq_cstr(&start_end_chop_str, "lo wo"));
}

void assert_fixed_strip_prefix_suffix()
{
    cvr_fixed_str base_str;
    cvr_fixed_str start_chop_str;
    cvr_fixed_str end_chop_str;

    cvr_fixed_str start_chop_check_str;
    cvr_fixed_str end_chop_check_str;

    base_str = cvr_fixed_from_cstr("Hello world!");
    start_chop_check_str = cvr_fixed_from_cstr("lo world!");
    end_chop_check_str = cvr_fixed_from_cstr("Hello wo");
    
    start_chop_str = cvr_fixed_strip_before(&base_str, 3);
    end_chop_str = cvr_fixed_strip_after(&base_str, 8);

    assert(cvr_fixed_eq(&start_chop_str, &start_chop_check_str));

    assert(cvr_fixed_eq(&start_chop_str, &start_chop_check_str));
}

void assert_fixed_starts_ends_with()
{
    cvr_fixed_str base_str;
    cvr_fixed_str starts_with_str;
    cvr_fixed_str ends_with_str;

    base_str = cvr_fixed_from_cstr("Hello world!");
    starts_with_str = cvr_fixed_from_cstr("Hel");
    ends_with_str = cvr_fixed_from_cstr("ld!");

    assert(cvr_fixed_starts_with(&base_str, &starts_with_str));
    assert(cvr_fixed_starts_with_cstr(&base_str, "Hel"));

    assert(cvr_fixed_ends_with(&base_str, &ends_with_str));
    assert(cvr_fixed_ends_with_cstr(&base_str, "ld!"));
}

void assert_fixed_split_words()
{
    cvr_fixed_str base_str;
    cvr_split_str words;
    cvr_fixed_str word;
    
    static const char *expected[] = {
        "This", "is", "a", "sentence", "with",
        "many", "many", "words."
    };
    const size_t expected_count = sizeof(expected) / sizeof(expected[0]);
    
    base_str = cvr_fixed_from_cstr("This is a sentence with many many words.");

    words = cvr_fixed_new_split(&base_str);

    for (int i = 0; i < expected_count; ++i) {
        assert(words.len > 0);

        word = cvr_split_next_word(&words);
        assert(cvr_fixed_eq_cstr(&word, expected[i]));
    }
}

void assert_fixed_split_lines()
{
    cvr_fixed_str base_str;
    cvr_split_str lines;
    cvr_fixed_str line;
    
    static const char *expected[] = {
        "This is a", "sentence", "with many",
        "many", "lines."
    };
    const size_t expected_count = sizeof(expected) / sizeof(expected[0]);

    base_str = cvr_fixed_from_cstr("This is a\nsentence\nwith many\nmany\nlines.");
    
    lines = cvr_fixed_new_split(&base_str);


    for (int i = 0; i < expected_count; ++i) {
        assert(lines.len > 0);

        line = cvr_split_next_line(&lines);
        assert(cvr_fixed_eq_cstr(&line, expected[i]));
    }
}

void assert_fixed_to_upper_lowercase()
{
    cvr_fixed_str base_str;
    cvr_dynamic_str uppercase_str;
    cvr_dynamic_str lowercase_str;

    cvr_fixed_str uppercase_check_str;
    cvr_fixed_str lowercase_check_str;
    
    base_str = cvr_fixed_from_cstr("Hello WORLD!");
    uppercase_check_str = cvr_fixed_from_cstr("HELLO WORLD!");
    lowercase_check_str = cvr_fixed_from_cstr("hello world!");

    cvr_fixed_to_uppercase_init(&base_str, &uppercase_str);
    cvr_fixed_to_lowercase_init(&base_str, &lowercase_str);

    assert(cvr_fixed_eq((cvr_fixed_str*)&uppercase_str, &uppercase_check_str));
    assert(cvr_fixed_eq_cstr((cvr_fixed_str*)&uppercase_str, "HELLO WORLD!"));

    assert(cvr_fixed_eq((cvr_fixed_str*)&lowercase_str, &lowercase_check_str));
    assert(cvr_fixed_eq_cstr((cvr_fixed_str*)&lowercase_str, "hello world!"));

    cvr_dynamic_uninit(&uppercase_str);
    cvr_dynamic_uninit(&lowercase_str);
}

void assert_fixed_searcher()
{
    cvr_fixed_str haystack;
    cvr_fixed_str needle;
    cvr_str_searcher twoWaySearcher;
    int result;
    int totalMatches = 0;
    size_t end = 0;

    haystack = cvr_fixed_from_cstr("This many is a sentence with many many words and even more words");
    needle = cvr_fixed_from_cstr("many");
    
    twoWaySearcher = cvr_str_searcher_new_search(&haystack, &needle);
 
    /*
    printf("searcher crit pos: %d\n", twoWaySearcher.critPos);
    printf("searcher crit pos back: %d\n", twoWaySearcher.critPosBack);
    printf("searcher period: %d\n", twoWaySearcher.period);
    printf("searcher byteset: %zu\n", twoWaySearcher.byteset);
    printf("searcher pos: %d\n", twoWaySearcher.pos);
    printf("searcher end: %d\n", twoWaySearcher.end);
    printf("searcher memory: %zu\n", twoWaySearcher.memory);
    printf("searcher memory back: %zu\n", twoWaySearcher.memoryBack);
    */

    for (;;) {
        cvr_search_match match;
        
        result = cvr_str_searcher_next(&twoWaySearcher, &match);
        if (result == -1) {
            break;
        }
        totalMatches++;
        assert(end <= match.start);
        end = match.end;

        assert(match.start == match.end - 4);
        assert(cvr_fixed_eq(&needle, (cvr_fixed_str*)&match));
    }
    assert(totalMatches == 3);
}

void assert_fixed_trim_nonalpha()
{
    cvr_fixed_str base_str;
    cvr_fixed_str trimmed_str;

    cvr_fixed_str trimmed_check_str;

    base_str = cvr_fixed_from_cstr("1. Hello world!");
    trimmed_str = cvr_fixed_from_cstr("Hello world!");

    trimmed_check_str = cvr_fixed_trim_nonalpha_start(&base_str);

    assert(cvr_fixed_eq(&trimmed_str, &trimmed_check_str));
}

void assert_fixed_contains()
{
    cvr_fixed_str base_str;
    cvr_fixed_str needle;
    cvr_fixed_str wrongNeedle;
    int result;

    base_str = cvr_fixed_from_cstr("This many is a sentence with many many words and even more words");
    wrongNeedle = cvr_fixed_from_cstr("assgfkdhjfg");
    needle = cvr_fixed_from_cstr("many");

    assert(cvr_fixed_contains(&base_str, &needle));
    assert(!cvr_fixed_contains(&base_str, &wrongNeedle));
    assert(!cvr_fixed_contains_start_at(&base_str, &needle, 40));
}

void assert_fixed_take_numeric_start()
{
    cvr_fixed_str base_str;
    cvr_fixed_str trimmed_str;
    uint64_t check = 10022;
    uint64_t trimmed_num;

    base_str = cvr_fixed_from_cstr("10022. Hello world!");
    trimmed_num = cvr_fixed_take_numeric_start(&base_str);
    
    assert(check == trimmed_num);
}

void assert_fixed_next_until_delim_cstr()
{
    cvr_fixed_str base_str;
    cvr_split_str split;
    int result, matches = 0;


    base_str = cvr_fixed_from_cstr("This many is a sentence with many many words and even more words.");

    split = cvr_fixed_new_split(&base_str);

    while (split.len > 0) {
        cvr_fixed_str words;
        
        words = cvr_split_next_until_delim_cstr(&split, "many");

        matches += 1;
    }
    assert(matches == 4);
}

void assert_fixed_next_until_delim()
{
    cvr_fixed_str base_str;
    cvr_fixed_str needle;
    cvr_split_str split;
    int result, matches = 0;

    base_str = cvr_fixed_from_cstr("This many is a sentence with many many words and even more words.");
    needle = cvr_fixed_from_cstr("many");

    split = cvr_fixed_new_split(&base_str);

    while (split.len > 0) {
        cvr_fixed_str words;
        
        cvr_fixed_printfln(&words);

        matches += 1;
    }
    assert(matches == 4);
}

void main() {
    assert_fixed_eq();
    assert_fixed_trim_whitespace();
    assert_fixed_preffix_suffix_chop();
    assert_fixed_strip_prefix_suffix();
    assert_fixed_starts_ends_with();
    assert_fixed_split_words();
    assert_fixed_split_lines();
    assert_fixed_to_upper_lowercase();
    assert_fixed_searcher();
    assert_fixed_trim_nonalpha();
    assert_fixed_take_numeric_start();
    assert_fixed_contains();
    assert_fixed_next_until_delim_cstr();
    assert_fixed_next_until_delim();

    printf("ALL TESTS PASS\n");
}
