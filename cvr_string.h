/*
This software is available as a choice of the following licenses. Choose
whichever you prefer.

===============================================================================
ALTERNATIVE 1 - Public Domain (www.unlicense.org)
===============================================================================
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

===============================================================================
ALTERNATIVE 2 - MIT No Attribution
===============================================================================
Copyright 2026 Cristian Roman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
    A string implementation in C89

    cvr_string - v0.1.0 21 Oct 2026
    
    Cristian Roman - romancristianv@gmail.com
    Github: http://github.com/someotherself/cvr_libs

    1. How to use
    
    You can add cvr_string to you project by adding this to your project entry point:
    
    #define CVRS_IMPLEMENTATION
    #include "path/to/cvr_string.h"

    In any other files where you are using the library, you only need to add:
    #include "path/to/cvr_string.h"

    2. How it works

    This library exposes 2 main types for you to work with.
    - cvr_fixed_str
    - cvr_dynamic_str

    Most functions are available for both types, however a cvr_dynamic_str
    can also be cast to a cvr_fixed_str.

    Some functions that take an index or a position, will take an `int`
    instead of `size_t`. This means that negative numbers can be used to
    index from the end. For example index `-1` takes the last character.
    
    The cvr_dynamic_str will allocate internally. Any function with `*_init()`
    must be matched with its corresponding `*_uninit()` function.

    2.1 cvr_fixed_str

    This is a lightweight handle that does not own the string.
    It can be viewed as a borrowed view into an existing (either stack or heap)
    allocated string. It never modifies the original string directly.
    Instead, functions returns a new cvr_fixed_str with a different view into
    the original string.

    2.1.1 cvr_fixed_str functions
    
    - Trimming & stripping
    
        Starting or ending whitespace can be trimmed with these functions.
        The functions is the standard library `isspace` to idenfity it.

            cvr_fixed_trim_whitespace_start(const cvr_fixed_str *pString);
            cvr_fixed_trim_whitespace_end(const cvr_fixed_str *pString);

        Non alpha starting characthers can be trimmed. All characters
        with ASCII value less than 64 will be trimmed.

            cvr_fixed_trim_nonalpha_start(const cvr_fixed_str *pString);

        The start or end can also be trimmed by using a fixed index. The
        returns string does not include characters until or after the index
        respectively.

            cvr_fixed_strip_before(const cvr_fixed_str *pString, int index);
            cvr_fixed_strip_after(const cvr_fixed_str *pString, int index);

    - Slicing

        These functions return a new string only containing characters until,
        after, or inbetween indexes. Take until functions do not include the
        index (0 based indexing) and take after functions include the starting
        index.
        Take numeric start uses the isdigit function to idetify and return any
        starting numbers as uint64_t.

            cvr_fixed_take_until(const cvr_fixed_str *pString, int index);
            cvr_fixed_take_after(const cvr_fixed_str *pString, int index);
            cvr_fixed_slice_between(const cvr_fixed_str *pString, size_t start, size_t end);
            cvr_fixed_take_numeric_start(const cvr_fixed_str *pString);

    - Equality checks

        Helpers exist to check equality between two cvr_fixedstr's, or against a cstr.

            cvr_fixed_eq(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
            cvr_fixed_eq_ignore_case(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
            cvr_fixed_eq_cstr(const cvr_fixed_str *pString, const char *pOtherString);
            cvr_fixed_eq_ignore_case_cstr(const cvr_fixed_str *pString, const char *pOtherString);

    - Strict matches

        These are more basic string matching functions that only check the start
        or end of a string.

            cvr_fixed_starts_with(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
            cvr_fixed_starts_with_cstr(const cvr_fixed_str *pString, const char *pCstr);
            cvr_fixed_ends_with(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
            cvr_fixed_ends_with_cstr(const cvr_fixed_str *pString, const char *pCstr);

    - Printing

        Since the cvr_* string types may not be null terminated, or they may end
        before the null terminator, separate printing function are available.
        They can print null terminators aswell, but this does not mean it will
        actually be displayed, depending on where you are printing it.

        printfln functions add a new line character after the string before
        printing it.

        These functions use fwrite to print to a file descriptor.
        They return -1 on error and do not set errno, but will set the error
        indicator of the stream.

            cvr_fixed_printf(const cvr_fixed_str *pString);
            cvr_fixed_printfln(const cvr_fixed_str *pString);
            cvr_fixed_fprintf(const cvr_fixed_str *pString, FILE *pFile);
            cvr_fixed_fprintf(const cvr_fixed_str *pString, FILE *pFile);

    - Uppercase and lowercase

        A string can be converter to all uppercase or all lowercase.
        These functions return a cvr_dynamic_str, which contains a newly
        allocated string. cvr_dynamic_uninit must be called to free the pOutput.

            cvr_fixed_to_uppercase_init(const cvr_fixed_str *pString, cvr_dynamic_str *pOutput);
            cvr_fixed_to_lowercase_init(const cvr_fixed_str *pString, cvr_dynamic_str *pOutput);

    - Matching

        A searching algorithm can be used to check if a string contains another cvr_fixed_str
        or a cstr.
        This uses the two way searching algorith. This algorithm can also be used directly,
        as explained further in the docs.

            cvr_fixed_contains(cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
            cvr_fixed_contains_start_at(cvr_fixed_str *pString, const cvr_fixed_str *pOtherString, int index);

    - String iterators
    
        The cvr_split_str is a type that allows to iterate over words, lines or
        between 
        This starts a lazy iterator and the all the iterator functions can be
        called in any order on the new cvr_split_str.
        
            cvr_fixed_new_split(const cvr_fixed_str *pString);
        
            cvr_split_next_word(cvr_split_str *pSplitStr);
            cvr_split_next_line(cvr_split_str *pSplitStr);
            cvr_split_next_until_delim(cvr_split_str *pSplitStr, const char* delim);
        
        Example:

            cvr_fixed_str base_str, word;
            cvr_split_str words;

            base_str = cvr_fixed_from_cstr("This is a sentence with many many words.");
            words = cvr_fixed_new_split(&base_str);
        
            while (words.len > 0) {
                word = cvr_split_next_word(&words);
                cvr_fixed_printfln(&word);
            }

    2.2 cvr_dynamic_str

        This is a string that own the allocation. When creating it from
        an existing string, it will copy it to a new allocation.
        This allows modifications to be made to the string, such as appending
        new characters.

    2.2.1 cvr_dynamic_str functions

        Most of the functions available on this type, are always available on
        cvr_fixed_str, by casting and delegating. I will only mention the
        functions unique to this type here.

    - Creating a new string

        An existing string can be converted to a cvr_dynamic_str using:
            cvr_dynamic_from_parts_init(const char *pdata, size_t len, cvr_dynamic_str *pstring);
            cvr_dynamic_from_cstr_init(const char *pcstr, cvr_dynamic_str *pstring);

        An empty string an also be created, and then text appended later:
            
            cvr_dynamic_with_capacity_init(size_t capacity, cvr_dynamic_str *pstring);

        These functions must be paired with the uninit to free memory:
            cvr_dynamic_uninit(cvr_dynamic_str *pString);

    - Appending
            
        Either a cvr_fixed_str can be appended or a cstr can be appended at the
        end, or after a specific index.
            
            cvr_dynamic_append(cvr_dynamic_str *pString, const cvr_fixed_str *pOtherString);
            cvr_dynamic_append_cstr(cvr_dynamic_str *pString, const char *pCstr);
            cvr_dynamic_append_at(cvr_dynamic_str *pString, const cvr_fixed_str *pOtherString, int index);
            cvr_dynamic_append_cstr_at(cvr_dynamic_str *pString, const char *pCstr, int index);

    3.0 cvr_str_searcher

    This library exposes an API to use the searching algorithm directly,
    and get matches one by one by calling next on an iterator.

    The iterator returns a cvr_search_match which can be cast to a
    cvr_fixed_str and it also contains the start and end indexes.
    These are absolute indexes into the haystack.

    Example:

    cvr_fixed_str haystack, needle;
    cvr_str_searcher twoWaySearcher;
    int result;

    haystack = cvr_fixed_from_cstr("This is a sentence with many words and even many more words");
    needle = cvr_fixed_from_cstr("many");
    
    twoWaySearcher = cvr_str_searcher_new_search(&haystack, &needle);

    for (;;) {
        cvr_search_match match;

        result = cvr_str_searcher_next(&twoWaySearcher, &match);
        if (result == -1) {
            break;
        }

        printf("Found match at pos: %ld\n", match.start);
    }
*/

#ifndef CVRS_H
#define CVRS_H

#define CVRS_STRINGIFY(x)     #x
#define CVRS_XSTRINGIFY(x)    CVRS_STRINGIFY(x)

#define CVRS_VERSION_MAJOR      0
#define CVRS_VERSION_MINOR      1
#define CVRS_VERSION_REVISION   0
#define CVRS_VERSION_STRING     CVRS_XSTRINGIFY(CVRS_VERSION_MAJOR) "." CVRS_XSTRINGIFY(CVRS_VERSION_MINOR) "." CVRS_XSTRINGIFY(CVRS_VERSION_REVISION)

#define CVRW_MAX(a, b) ((a) > (b) ? (a) : (b))

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef CVRS_API
#define CVRS_API
#endif

typedef uint8_t cvrs_bool8;
#define CVRS_TRUE   1
#define CVRS_FALSE  0

/* We don't double the capacity on grow. Instead we grow by a fixed size */
#define CAP_GROW_SIZE   256

typedef struct cvr_fixed_str cvr_split_str;
typedef struct cvr_fixed_str cvr_fixed_str;

struct cvr_fixed_str {
    const void *pData;
    size_t len;
};

typedef struct cvr_dynamic_str cvr_dynamic_str;
struct cvr_dynamic_str {
    cvr_fixed_str string;
    size_t capacity;
};

CVRS_API cvr_fixed_str cvr_fixed_from_parts(const char *pData, size_t len);
CVRS_API cvr_fixed_str cvr_fixed_from_cstr(const char *pCstr);
CVRS_API int cvr_fixed_to_cstr(const cvr_fixed_str *pCstr, char *dest);

CVRS_API cvr_fixed_str cvr_fixed_trim_whitespace_start(const cvr_fixed_str *pString);
CVRS_API cvr_fixed_str cvr_fixed_trim_whitespace_end(const cvr_fixed_str *pString);
CVRS_API cvr_fixed_str cvr_fixed_trim_nonalpha_start(const cvr_fixed_str *pString);

CVRS_API cvr_fixed_str cvr_fixed_take_until(const cvr_fixed_str *pString, int index);
CVRS_API cvr_fixed_str cvr_fixed_take_after(const cvr_fixed_str *pString, int index);
CVRS_API cvr_fixed_str cvr_fixed_slice_between(const cvr_fixed_str *pString, size_t start, size_t end);
CVRS_API uint64_t cvr_fixed_take_numeric_start(const cvr_fixed_str *pString);

CVRS_API cvr_fixed_str cvr_fixed_strip_before(const cvr_fixed_str *pString, int index);
CVRS_API cvr_fixed_str cvr_fixed_strip_after(const cvr_fixed_str *pString, int index);

CVRS_API cvrs_bool8 cvr_fixed_eq(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
CVRS_API cvrs_bool8 cvr_fixed_eq_ignore_case(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
CVRS_API cvrs_bool8 cvr_fixed_eq_cstr(const cvr_fixed_str *pString, const char *pOtherString);
CVRS_API cvrs_bool8 cvr_fixed_eq_ignore_case_cstr(const cvr_fixed_str *pString, const char *pOtherString);

CVRS_API cvrs_bool8 cvr_fixed_starts_with(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
CVRS_API cvrs_bool8 cvr_fixed_starts_with_cstr(const cvr_fixed_str *pString, const char *pCstr);
CVRS_API cvrs_bool8 cvr_fixed_ends_with(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
CVRS_API cvrs_bool8 cvr_fixed_ends_with_cstr(const cvr_fixed_str *pString, const char *pCstr);

CVRS_API int cvr_fixed_printf(const cvr_fixed_str *pString);
CVRS_API int cvr_fixed_printfln(const cvr_fixed_str *pString);
CVRS_API int cvr_fixed_fprintf(const cvr_fixed_str *pString, FILE *pFile);
CVRS_API int cvr_fixed_fprintfln(const cvr_fixed_str *pString, FILE *pFile);

CVRS_API cvr_split_str cvr_fixed_new_split(const cvr_fixed_str *pString);

CVRS_API cvrs_bool8 cvr_fixed_contains(cvr_fixed_str *pString, const cvr_fixed_str *pOtherString);
CVRS_API cvrs_bool8 cvr_fixed_contains_start_at(cvr_fixed_str *pString, const cvr_fixed_str *pOtherString, int index);

CVRS_API int cvr_fixed_to_uppercase_init(const cvr_fixed_str *pString, cvr_dynamic_str *pOutput);
CVRS_API int cvr_fixed_to_lowercase_init(const cvr_fixed_str *pString, cvr_dynamic_str *pOutput);

/* cvr_dynamic_str functions  */

CVRS_API int cvr_dynamic_with_capacity_init(size_t capacity, cvr_dynamic_str *pstring);
CVRS_API int cvr_dynamic_from_parts_init(const char *pdata, size_t len, cvr_dynamic_str *pstring);
CVRS_API int cvr_dynamic_from_cstr_init(const char *pcstr, cvr_dynamic_str *pstring);
CVRS_API void cvr_dynamic_uninit(cvr_dynamic_str *pString);

CVRS_API int cvr_dynamic_to_cstr(const cvr_dynamic_str *pString, char *dest);

CVRS_API cvr_fixed_str cvr_dynamic_trim_whitespace_start(const cvr_dynamic_str *pString);
CVRS_API cvr_fixed_str cvr_dynamic_trim_whitespace_end(const cvr_dynamic_str *pString);

CVRS_API cvrs_bool8 cvr_dynamic_eq(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString);
CVRS_API cvrs_bool8 cvr_dynamic_eq_ignore_case(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString);
CVRS_API cvrs_bool8 cvr_dynamic_eq_cstr(const cvr_dynamic_str *pString, const char *pOtherString);
CVRS_API cvrs_bool8 cvr_dynamic_eq_ignore_case_cstr(const cvr_dynamic_str *pString, const char *pCstr);

CVRS_API cvrs_bool8 cvr_dynamic_starts_with(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString);
CVRS_API cvrs_bool8 cvr_dynamic_starts_with_cstr(const cvr_dynamic_str *pString, const char *pCstr);
CVRS_API cvrs_bool8 cvr_dynamic_ends_with(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString);
CVRS_API cvrs_bool8 cvr_dynamic_ends_with_cstr(const cvr_dynamic_str *pString, const char *pCstr);

CVRS_API int cvr_dynamic_printf(const cvr_dynamic_str *pString);
CVRS_API int cvr_dynamic_printfln(const cvr_dynamic_str *pString);
CVRS_API int cvr_dynamic_fprintf(const cvr_dynamic_str *pString, FILE *pFile);
CVRS_API int cvr_dynamic_fprintfln(const cvr_dynamic_str *pString, FILE *pFile);

CVRS_API int cvr_dynamic_append(cvr_dynamic_str *pString, const cvr_fixed_str *pOtherString);
CVRS_API int cvr_dynamic_append_cstr(cvr_dynamic_str *pString, const char *pCstr);
CVRS_API int cvr_dynamic_append_at(cvr_dynamic_str *pString, const cvr_fixed_str *pOtherString, int index);
CVRS_API int cvr_dynamic_append_cstr_at(cvr_dynamic_str *pString, const char *pCstr, int index);

CVRS_API cvr_split_str cvr_dynamic_new_split(const cvr_dynamic_str *pString);

CVRS_API int cvr_dynamic_to_uppercase_init(const cvr_dynamic_str *pString, cvr_dynamic_str *pOutput);
CVRS_API int cvr_dynamic_to_lowercase_init(const cvr_dynamic_str *pString, cvr_dynamic_str *pOutput);

/* cvr_split_str Iterator functions  */

CVRS_API cvr_fixed_str cvr_split_next_word(cvr_split_str *pSplitStr);
CVRS_API cvr_fixed_str cvr_split_next_line(cvr_split_str *pSplitStr);
CVRS_API cvr_fixed_str cvr_split_next_until_delim(cvr_split_str *pSplitStr, const cvr_split_str *delim);
CVRS_API cvr_fixed_str cvr_split_next_until_delim_cstr(cvr_split_str *pSplitStr, const char *delim);

typedef struct cvr_search_match cvr_search_match;

struct cvr_search_match {
    cvr_fixed_str string;
    size_t start;
    size_t end;
};

typedef struct cvr_str_searcher cvr_str_searcher;
typedef struct cvr_fixed_str cvr_str_matches;

struct cvr_str_searcher {
    const cvr_fixed_str *pString;
    const cvr_fixed_str *pPattern;
    /* Critical factorization index */
    size_t critPos;
    /* Critical factorization index for reversed needle */
    size_t critPosBack;
    size_t period;
    /*
    Byteset is an extension (not in the algorithm):
    a 64-bit fingerprint where each set bit 'j' corresponds
    to a (byte & 63) == j present in the needle
    */
    uint64_t byteset;

    /* Variables */
    size_t pos;
    size_t end;
    /* Index into the needle before which we have already matched */
    size_t memory;
    /* Index into the needle after which we have already matched */
    size_t memoryBack;
};

CVRS_API cvr_str_searcher cvr_str_searcher_new_search(const cvr_fixed_str *pString, const cvr_fixed_str *pPattern);
CVRS_API int cvr_str_searcher_next(cvr_str_searcher *pSearch, cvr_search_match *pOutMatch);
CVRS_API int cvr_str_searcher_find_first(const cvr_fixed_str *pString, const cvr_fixed_str *pPattern, cvr_search_match *pOutMatch);

#endif /* CVR_S_H */

#ifdef CVRS_IMPLEMENTATION

static size_t next_multiple_of(size_t value, size_t multiple)
{
    size_t remainder = 0;

    if (multiple == 0) {
        return 0;
    }
    
    remainder = value % multiple;

    if (remainder == 0) {
        return value;
    } else {
        return value + (multiple - remainder);
    }
}

static int write_all_to_fd(const char *pData, size_t len, FILE *pFile)
{
    size_t result;
    size_t written = 0;

    if (len == 0) {
        return 0;
    }

    while (written < len) {
        result = fwrite(pData + written, 1, len - written, pFile);

        written += result;

        if (written != len) {
            if (ferror(pFile) && errno == EINTR) {
                clearerr(pFile);
                continue;
            }

            return -1;
        }
    }

    return 0;
}

/* Used by the two way string search algorithm */
static void maximal_suffix(const cvr_fixed_str *pPat, cvrs_bool8 orderGreater, size_t* startIdx, size_t *p)
{
    size_t left = 0; /* Corresponds to i in the paper */
    size_t right = 1; /* Corresponds to j in the paper */
    size_t offset = 0; /* Corresponds to k in the paper, starting at 0 */
    size_t period = 1; /* Corresponds to p in the paper */

    char a, b; /* temporary left and right inside the loop */

    char* pattern = (char*)pPat->pData;
    size_t len = pPat->len;

    size_t i = 0;
    for (;;) {
        /* `-1` because we must use `right + offset` to index */
        if ((right + offset) > len - 1 || len == 0) {
            break;
        }

        a = pattern[right + offset];
        b = pattern[left + offset];

        if ((a < b && !orderGreater) || (a > b && orderGreater)) {
            /* Suffix is smaller, period is entire prefix so far  */
            right += offset + 1;
            offset = 0;
            period = right - left;

        } else if (a == b) {
            /* Advance through repetition (current occurrence) of current period */
            if ((offset + 1) == period) {
                right += offset + 1;
                offset = 0;
            } else {
                offset++;
            }
        } else {
            /* Suffix is larger, start over from current location */
            left = right;
            right++;
            offset = 0;
            period = 1;
        }
        i++;
    }
    *startIdx = left;
    *p = period;
}

/* Used by the two way string search algorithm */
static void reverse_maximal_suffix(const cvr_fixed_str *pPat, size_t knownPeriod, cvrs_bool8 orderGreater, size_t* startIdx)
{
    size_t left = 0; /* Corresponds to i in the paper */
    size_t right = 1; /* Corresponds to j in the paper */
    size_t offset = 0; /* Corresponds to k in the paper, starting at 0 */
    size_t period = 1; /* Corresponds to p in the paper */

    char* pattern = (char*)pPat->pData;
    size_t len = pPat->len;

    char a, b; /* temporary left and right inside the loop */

    while (right + offset < len) {
        a = pattern[len - 1 + right + offset];
        b = pattern[len - 1 + left + offset];
    
        if ((a < b && !orderGreater) || (a > b && orderGreater)) {
            /* Suffix is smaller, period is entire prefix so far */
            right += offset + 1;
            offset = 0;
            period = right - left;
        } else if (a == b) {
            /* Advance through repetition of the current period */
            if (offset + 1 == period) {
                right += offset + 1;
                offset = 0;
            } else {
                offset += 1;
            }
        } else {
            /* Suffix is larger, start over from current location */
            left = right;
            right += 1;
            offset = 0;
            period = 1;
        }
        if (period == knownPeriod) {
            break;
        }
    }
    assert(period <= knownPeriod);
    *startIdx = left;
}

/* cvr_fixed_str functions */

CVRS_API cvr_fixed_str cvr_fixed_from_parts(const char *pData, size_t len)
{
    cvr_fixed_str string;
    string.len = len;
    string.pData = (void*)pData;
    return string;
}

CVRS_API cvr_fixed_str cvr_fixed_from_cstr(const char *pCstr)
{
    return cvr_fixed_from_parts(pCstr, strlen(pCstr));
}

CVRS_API int cvr_fixed_to_cstr(const cvr_fixed_str *pString, char *dest)
{
    size_t i = 0;

    if (pString == NULL || dest == NULL) {
        return -1;
    }

    while (i < pString->len) {
        dest[i] = *((char*)pString->pData + i);
        i++;
    }
    dest[i] = '\0';

    return 0;
}

CVRS_API cvr_fixed_str cvr_fixed_trim_whitespace_start(const cvr_fixed_str *pString)
{
    size_t i = 0;

    while (i < pString->len && isspace(*((char*)pString->pData + i))) {
        i += 1;
    }

    return cvr_fixed_from_parts((char*)pString->pData + i, pString->len - i);
}

CVRS_API cvr_fixed_str cvr_fixed_trim_whitespace_end(const cvr_fixed_str *pString)
{
    size_t i = 0;
    size_t len = pString->len;

    while (i < len && isspace(*((char*)pString->pData + len - 1 - i))) {
        i += 1;
    }

    return cvr_fixed_from_parts((char*)pString->pData, pString->len - i);
}

CVRS_API cvr_fixed_str cvr_fixed_trim_nonalpha_start(const cvr_fixed_str *pString)
{
    size_t i = 0;

    while (i < pString->len && *((char*)pString->pData + i) < 64) {
        i++;
    }

    return cvr_fixed_from_parts((char*)pString->pData + i, pString->len - i);
}

CVRS_API cvr_fixed_str cvr_fixed_take_until(const cvr_fixed_str *pString, int index)
{
    int pos = index;

    assert(abs(index) <= pString->len);
    if (index < 0) {
        /* We don't want to include the index */
        pos = pString->len + index - 1;
    }

    return cvr_fixed_from_parts((char*)pString->pData, pos);
}

CVRS_API cvr_fixed_str cvr_fixed_take_after(const cvr_fixed_str *pString, int index)
{
    int pos = index;

    assert(abs(index) <= pString->len);
    if (index < 0) {
        pos = pString->len + index;
    }

    return cvr_fixed_from_parts((char*)pString->pData + pos, pString->len - pos);
}

CVRS_API cvr_fixed_str cvr_fixed_slice_between(const cvr_fixed_str *pString, size_t start, size_t end)
{
    assert(end <= pString->len);
    assert(start <= end);

    return cvr_fixed_from_parts((char*)pString->pData + start, end - start);
}

CVRS_API uint64_t cvr_fixed_take_numeric_start(const cvr_fixed_str *pString)
{
    size_t i = 0;
    uint64_t result = 0;
    const char* string = (const char*)pString->pData;

    while (i < pString->len && isdigit(string[i])) {
        result = result * 10 + string[i] - '0';
        i++;
    }
    
    return result;
}

CVRS_API cvr_fixed_str cvr_fixed_strip_before(const cvr_fixed_str *pString, int index)
{
    size_t pos = index;

    assert(abs(index) <= pString->len);
    if (index < 0) {
        pos = pString->len + index;
    }

    return cvr_fixed_from_parts((char*)pString->pData + pos, pString->len - pos);
}

CVRS_API cvr_fixed_str cvr_fixed_strip_after(const cvr_fixed_str *pString, int index)
{
    size_t pos = index;

    assert(abs(index) <= pString->len);
    if (index < 0) {
        /* We don't want to include the index */
        pos = pString->len + index - 1;
    }

    return cvr_fixed_from_parts((char*)pString->pData, pString->len - pos);
}

CVRS_API cvrs_bool8 cvr_fixed_eq(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString)
{
    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    if (pString->len != pOtherString->len) {
        return CVRS_FALSE;
    } else {
        return memcmp(pString->pData, pOtherString->pData, pString->len) == 0 ? CVRS_TRUE : CVRS_FALSE;
    }
}

CVRS_API cvrs_bool8 cvr_fixed_eq_ignore_case(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString)
{
    size_t i = 0;
    const char* string = (const char*)pString->pData;
    const char* otherString = (const char*)pOtherString->pData;

    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    if (pString->len != pOtherString->len) {
        return CVRS_FALSE;
    } else {
        char x, y;

        while (i < pString->len) {
            x = 'A' <= *(string + i) && *(string + i) <= 'Z'
                ? *(string + i) + 32
                : *(string + i);
            y = 'A' <= *(otherString + i) && *(otherString + i) <= 'Z'
                ? *(otherString + i) + 32
                : *(otherString + i);

            if (x != y) return CVRS_FALSE;
            i++;
        }
        return CVRS_TRUE;
    }
}

CVRS_API cvrs_bool8 cvr_fixed_eq_cstr(const cvr_fixed_str *pString, const char *pCstr)
{
    cvr_fixed_str pOtherString;

    if (pString == NULL) {
        return CVRS_FALSE;
    }

    pOtherString = cvr_fixed_from_cstr(pCstr);

    return cvr_fixed_eq(pString, &pOtherString);
}

CVRS_API cvrs_bool8 cvr_fixed_eq_ignore_case_cstr(const cvr_fixed_str *pString, const char *pCstr)
{
    cvr_fixed_str pOtherString;

    if (pString == NULL) {
        return CVRS_FALSE;
    }

    pOtherString = cvr_fixed_from_cstr(pCstr);

    return cvr_fixed_eq_ignore_case(pString, &pOtherString);
}

CVRS_API cvrs_bool8 cvr_fixed_starts_with(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString)
{
    cvr_fixed_str trimmed;

    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    if (pOtherString->len <= pString->len) {
        trimmed = cvr_fixed_from_parts((char*)pString->pData, pOtherString->len);
        return cvr_fixed_eq(pOtherString, &trimmed);
    }
    
    return CVRS_FALSE;
}

CVRS_API cvrs_bool8 cvr_fixed_starts_with_cstr(const cvr_fixed_str *pString, const char *pCstr)
{
    cvr_fixed_str otherString;

    if (pString == NULL) {
        return CVRS_FALSE;
    }

    otherString = cvr_fixed_from_parts(pCstr, strlen(pCstr));

    return cvr_fixed_starts_with(pString, &otherString);
}

CVRS_API cvrs_bool8 cvr_fixed_ends_with(const cvr_fixed_str *pString, const cvr_fixed_str *pOtherString)
{
    cvr_fixed_str trimmed;

    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    if (pOtherString->len <= pString->len) {
        trimmed = cvr_fixed_from_parts((char*)(pString->pData) + (pString->len - pOtherString->len), pOtherString->len);
        return cvr_fixed_eq(pOtherString, &trimmed);
    }

    return CVRS_FALSE;
}

CVRS_API cvrs_bool8 cvr_fixed_ends_with_cstr(const cvr_fixed_str *pString, const char *pCstr)
{
    cvr_fixed_str otherString;

    if (pString == NULL) {
        return CVRS_FALSE;
    }

    otherString = cvr_fixed_from_parts(pCstr, strlen(pCstr));

    return cvr_fixed_ends_with(pString, &otherString);
}

CVRS_API int cvr_fixed_printf(const cvr_fixed_str *pString)
{
    if (pString == NULL) {
        return -1;
    }

    return cvr_fixed_fprintf(pString, stdout);
}

CVRS_API int cvr_fixed_printfln(const cvr_fixed_str *pString)
{
    if (pString == NULL) {
        return -1;
    }

    return cvr_fixed_fprintfln(pString, stdout);
}

CVRS_API int cvr_fixed_fprintf(const cvr_fixed_str *pString, FILE *pFile)
{
    if (pString == NULL) {
        return -1;
    }

    return write_all_to_fd((char*)pString->pData, pString->len, pFile);
}

CVRS_API int cvr_fixed_fprintfln(const cvr_fixed_str *pString, FILE *pFile)
{
    char* tBuffer;
    int result;

    if (pString == NULL) {
        return -1;
    }

    tBuffer = (char*)malloc(pString->len + 1);

    if (pString->len > 0) {
         memcpy(tBuffer, (char*)pString->pData, pString->len);
    }
    tBuffer[pString->len] = '\n';

    result = write_all_to_fd(tBuffer, pString->len + 1, pFile);
    
    free(tBuffer);

    return result;
}

CVRS_API int cvr_fixed_to_uppercase_init(const cvr_fixed_str *pString, cvr_dynamic_str *pOutput)
{
    size_t i = 0;
    char x;
    int result;
    const char* string = (const char*)pString->pData;

    result = cvr_dynamic_with_capacity_init(pString->len, pOutput);
    if (result == -1) {
        return -1;
    }

    while (i < pString->len) {
        x = 'a' <= *(string + i) && *(string + i) <= 'z'
            ? *(string + i) - 32
            : *(string + i);

        memcpy((char*)(pOutput->string.pData) + i, &x, 1);
        i++;
    }
    pOutput->string.len = pString->len;

    return 0;
}

CVRS_API int cvr_fixed_to_lowercase_init(const cvr_fixed_str *pString, cvr_dynamic_str *pOutput)
{
    size_t i = 0;
    char x;
    int result;
    const char* string = (const char*)pString->pData;

    result = cvr_dynamic_with_capacity_init(pString->len, pOutput);
    if (result == -1) {
        return -1;
    }

    while (i < pString->len) {
        x = 'A' <= *(string + i) && *(string + i) <= 'Z'
            ? *(string + i) + 32
            : *(string + i);

        memcpy((char*)(pOutput->string.pData) + i, &x, 1);
        i++;
    }
    pOutput->string.len = pString->len;

    return 0;
}

CVRS_API cvr_split_str cvr_fixed_new_split(const cvr_fixed_str *pString)
{
    cvr_split_str split;
    cvr_fixed_str trimmed;

    trimmed = cvr_fixed_trim_whitespace_end(pString);

    split.pData = trimmed.pData;
    split.len = trimmed.len;

    return split;
}

CVRS_API cvrs_bool8 cvr_fixed_contains(cvr_fixed_str *pString, const cvr_fixed_str *pOtherString)
{
    int result;
    cvr_search_match match = { 0 }; /* we will discard this */

    result = cvr_str_searcher_find_first(pString, pOtherString, &match);

    if (result == 0) {
        return CVRS_TRUE;
    } else {
        return CVRS_FALSE;
    }
}

CVRS_API cvrs_bool8 cvr_fixed_contains_start_at(cvr_fixed_str *pString, const cvr_fixed_str *pOtherString, int index)
{
    int result;
    cvr_search_match match = { 0 }; /* we will discard this */
    cvr_fixed_str string;
    int pos = index;

    assert(abs(index) <= pString->len);
    if (index < 0) {
        pos = pString->len + index;
    }

    string = cvr_fixed_from_parts((char*)pString->pData + pos, pString->len - pos);

    result = cvr_str_searcher_find_first(&string, pOtherString, &match);

    if (result == 0) {
        return CVRS_TRUE;
    } else {
        return CVRS_FALSE;
    }
}

/* cvr_dynamic_str functions */

CVRS_API int cvr_dynamic_with_capacity_init(size_t capacity, cvr_dynamic_str *pString)
{
    size_t newCapacity;
    void* pData;

    newCapacity = next_multiple_of(capacity, CAP_GROW_SIZE);

    pData = malloc(capacity);
    if (pData == NULL) {
        return -1;
    }

    pString->string.pData = pData;
    pString->string.len = 0;
    pString->capacity = newCapacity;

    return 0;
}

CVRS_API int cvr_dynamic_from_parts_init(const char *pData, size_t len, cvr_dynamic_str *pString)
{
    size_t capacity;
    void* pNewData;

    if (pData == NULL) {
        return -1;
    }

    capacity = next_multiple_of(len, CAP_GROW_SIZE);
    
    pNewData = malloc(capacity);
    if (pNewData == NULL) {
        return -1;
    }

    /* Copy the text into the new allocation */
    memcpy(pNewData, pData, len);

    pString->string.pData = pNewData;
    pString->string.len = len;
    pString->capacity = capacity;

    return 0;
}

CVRS_API int cvr_dynamic_from_cstr_init(const char *pCstr, cvr_dynamic_str *pString)
{
    return cvr_dynamic_from_parts_init(pCstr, strlen(pCstr), pString);
}

CVRS_API void cvr_dynamic_uninit(cvr_dynamic_str *pString)
{
    if (pString == NULL) {
        return;
    }

    free((void*)pString->string.pData);
}

CVRS_API int cvr_dynamic_to_cstr(const cvr_dynamic_str *pString, char *dest)
{
    if (pString == NULL || dest == NULL) {
        return -1;
    }

    return cvr_fixed_to_cstr(&pString->string, dest);
}

CVRS_API cvr_fixed_str cvr_dynamic_trim_whitespace_start(const cvr_dynamic_str *pString)
{
    return cvr_fixed_trim_whitespace_start(&pString->string);
}

CVRS_API cvr_fixed_str cvr_dynamic_trim_whitespace_end(const cvr_dynamic_str *pString)
{
    return cvr_fixed_trim_whitespace_end(&pString->string);
}

CVRS_API cvrs_bool8 cvr_dynamic_eq(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString)
{
    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_eq(&pString->string, &pOtherString->string);
}

CVRS_API cvrs_bool8 cvr_dynamic_eq_ignore_case(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString)
{
    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_eq_ignore_case(&pString->string, &pOtherString->string);
}

CVRS_API cvrs_bool8 cvr_dynamic_eq_cstr(const cvr_dynamic_str *pString, const char *pOtherString)
{
    if (pString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_eq_cstr(&pString->string, pOtherString);
}

CVRS_API cvrs_bool8 cvr_dynamic_eq_ignore_case_cstr(const cvr_dynamic_str *pString, const char *pCstr)
{
    if (pString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_eq_ignore_case_cstr(&pString->string, pCstr);
}

CVRS_API cvrs_bool8 cvr_dynamic_starts_with(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString)
{
    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_starts_with(&pString->string, &pOtherString->string);
}

CVRS_API cvrs_bool8 cvr_dynamic_starts_with_cstr(const cvr_dynamic_str *pString, const char *pCstr)
{
    if (pString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_starts_with_cstr(&pString->string, pCstr);
}

CVRS_API cvrs_bool8 cvr_dynamic_ends_with(const cvr_dynamic_str *pString, const cvr_dynamic_str *pOtherString)
{
    if (pString == NULL || pOtherString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_ends_with(&pString->string, &pOtherString->string);
}

CVRS_API cvrs_bool8 cvr_dynamic_ends_with_cstr(const cvr_dynamic_str *pString, const char *pCstr)
{
    if (pString == NULL) {
        return CVRS_FALSE;
    }

    return cvr_fixed_ends_with_cstr(&pString->string, pCstr);
}

CVRS_API int cvr_dynamic_printf(const cvr_dynamic_str *pString)
{
    if (pString == NULL) {
        return -1;
    }

    return cvr_dynamic_fprintf(pString, stdout);
}

CVRS_API int cvr_dynamic_printfln(const cvr_dynamic_str *pString)
{
    if (pString == NULL) {
        return -1;
    }

    return cvr_dynamic_fprintfln(pString, stdout);
}

CVRS_API int cvr_dynamic_fprintf(const cvr_dynamic_str *pString, FILE *pFile)
{
    if (pString == NULL) {
        return -1;
    }

    return cvr_fixed_fprintf((const cvr_fixed_str*)pString, pFile);
}

CVRS_API int cvr_dynamic_fprintfln(const cvr_dynamic_str *pString, FILE *pFile)
{
    if (pString == NULL) {
        return -1;
    }

    return cvr_fixed_fprintfln((const cvr_fixed_str*)pString, pFile);
}

CVRS_API int cvr_dynamic_append(cvr_dynamic_str *pString, const cvr_fixed_str *pOtherString)
{
    size_t newLen;
    size_t totalCapacity;
    void* pNewData;

    if (pString == NULL || pOtherString == NULL) {
        return -1;
    }
    
    newLen = pString->string.len + pOtherString->len;
    totalCapacity = pString->capacity = pOtherString->len;
    
    if (newLen > totalCapacity) {
        totalCapacity = next_multiple_of(newLen, CAP_GROW_SIZE);

        pNewData = realloc((void*)pString->string.pData, newLen);
        if (pNewData == NULL) {
            return -1;
        }
        
        pString->string.pData = pNewData;
    }
    /* We've made sure that we have enough capacity to copy the new text */

    memcpy((char*)(pString->string.pData) + pString->string.len, (char*)(pOtherString->pData), pOtherString->len);
    pString->string.len = newLen;
    pString->capacity = totalCapacity;

    return 0;
}

CVRS_API int cvr_dynamic_append_cstr(cvr_dynamic_str *pString, const char *pCstr)
{
    cvr_fixed_str pOtherString;

    if (pString == NULL) {
        return -1;
    }

    pOtherString = cvr_fixed_from_cstr(pCstr);
    
    return cvr_dynamic_append(pString, &pOtherString);
}

CVRS_API int cvr_dynamic_append_at(cvr_dynamic_str *pString, const cvr_fixed_str *pOtherString, int index)
{
    size_t newLen;
    size_t totalCapacity;
    void* pNewData;
    size_t pos = abs(index);

    if (abs(index) > pString->string.len) {
        return -1;
    }

    if (pString == NULL || pOtherString == NULL) {
        return -1;
    }
 
    /* index can be negative, in which case, we count from the end */
    if (index < 0) {
        pos = pString->string.len + index;
    }   

    newLen = pString->string.len + pOtherString->len;
    totalCapacity = next_multiple_of(newLen, CAP_GROW_SIZE);

    pNewData = malloc(totalCapacity);
    if (pNewData == NULL) {
        return -1;
    }

    /* Copy the suffix, if any*/
    memcpy((char*)pNewData, (char*)(pString->string.pData), pos);

    /* Append the new text */
    memcpy((char*)pNewData + pos, (char*)(pOtherString->pData), pOtherString->len);

    /* Copy the prefix, if any */
    memcpy((char*)pNewData + pos + pOtherString->len, (char*)(pString->string.pData) + pos, pString->string.len - pos);

    free((void*)pString->string.pData);

    pString->string.pData = pNewData;
    pString->string.len = newLen;
    pString->capacity = totalCapacity;
    
    return 0;
}

CVRS_API int cvr_dynamic_append_cstr_at(cvr_dynamic_str *pString, const char *pCstr, int index)
{
    cvr_fixed_str pOtherString;

    if (pString == NULL) {
        return -1;
    }

    pOtherString = cvr_fixed_from_cstr(pCstr);
    
    return cvr_dynamic_append_at(pString, &pOtherString, index);
}

CVRS_API cvr_split_str cvr_dynamic_new_split(const cvr_dynamic_str *pString)
{
    return cvr_fixed_new_split((cvr_fixed_str*)pString);
}

CVRS_API int cvr_dynamic_to_uppercase_init(const cvr_dynamic_str *pString, cvr_dynamic_str *pOutput)
{
    return cvr_fixed_to_uppercase_init((cvr_fixed_str*)pString, pOutput);
}

CVRS_API int cvr_dynamic_to_lowercase_init(const cvr_dynamic_str *pString, cvr_dynamic_str *pOutput)
{
    return cvr_fixed_to_lowercase_init((cvr_fixed_str*)pString, pOutput);
}

CVRS_API cvr_fixed_str cvr_split_next_word(cvr_split_str *pSplitStr)
{
    size_t i = 0;
    size_t j = 0;
    cvr_fixed_str word;
    const char* string = (const char*)pSplitStr->pData;
    
    /* Find the start of the word */
    while (i < pSplitStr->len && isspace(string[i])) {
        i += 1;
    }

    /* Find the end of the word */
    j += i;
    while (j < pSplitStr->len && !isspace(string[j])) {
        j += 1;
    }

    word = cvr_fixed_from_parts((char*)(pSplitStr->pData) + i, j - i);
    pSplitStr->pData = (char*)(pSplitStr->pData) + j;
    pSplitStr->len = pSplitStr->len - j;

    return word;
}

CVRS_API cvr_fixed_str cvr_split_next_line(cvr_split_str *pSplitStr)
{
    size_t i = 0;
    size_t j = 0;
    cvr_fixed_str line;
    const char* string = (const char*)pSplitStr->pData;

    /* Find the start of the line */
    while (i < pSplitStr->len && isspace(string[i])) {
        i += 1;
    }
    
    /* Find the end of the line */
    j += i;
    while (j < pSplitStr->len && !(string[j] == '\n' || string[j] == '\r')) {
        j += 1;
    }

    line = cvr_fixed_from_parts((char*)(pSplitStr->pData)+ i, j - i);
    pSplitStr->pData = (char*)(pSplitStr->pData) + j;
    pSplitStr->len = pSplitStr->len - j;

    return line;
}

CVRS_API cvr_fixed_str cvr_split_next_until_delim(cvr_split_str *pSplitStr, const cvr_split_str *pDelim)
{
    cvr_str_searcher twoWaySearcher;
    cvr_fixed_str output;
    cvr_search_match match;
    int result;

    twoWaySearcher = cvr_str_searcher_new_search((cvr_fixed_str*)pSplitStr, pDelim);

    result = cvr_str_searcher_next(&twoWaySearcher, &match);
    if (result == -1) {
        /* return the rest of test */
        output = cvr_fixed_from_parts(pSplitStr->pData, pSplitStr->len);
        pSplitStr->pData = (char*)(pSplitStr->pData) + pSplitStr->len;
        pSplitStr->len = 0;
        return output;
    }
    
    /* We have found a match */
    output = cvr_fixed_from_parts(pSplitStr->pData, match.start);

    /* We want to skip past the match we found */
    pSplitStr->pData = (char*)(pSplitStr->pData) + match.end;
    pSplitStr->len = pSplitStr->len - match.end;

    return output;
}

CVRS_API cvr_fixed_str cvr_split_next_until_delim_cstr(cvr_split_str *pSplitStr, const char *pDelim)
{
    cvr_fixed_str needle;

    needle = cvr_fixed_from_cstr(pDelim);

    return cvr_split_next_until_delim(pSplitStr, &needle);
}

/* cvr_string_searcher functions */

CVRS_API cvr_str_searcher cvr_str_searcher_new_search(const cvr_fixed_str *pString, const cvr_fixed_str *pPattern)
{
    size_t critPosFalse, critPosTrue; /* temporary values */
    size_t periodFalse, periodTrue; /* temporary values */
    size_t tRevIdxFalse, tRevIdxTrue; /* temporary values, short period case */
    cvr_fixed_str leftStrCmp, rightStrCmp; /* temporary strings for comparisons */
    size_t i = 0; /* Needed for calculating byteset */

    /* Final values */
    size_t critPos, period;
    size_t critPosBack;
    uint64_t byteset = 0;
    cvr_str_searcher outSearcher;
    
    const char* pattern = (const char*)pPattern->pData;

    maximal_suffix(pPattern, CVRS_FALSE, &critPosFalse, &periodFalse);
    maximal_suffix(pPattern, CVRS_TRUE, &critPosTrue, &periodTrue);

    if (critPosFalse > critPosTrue) {
        critPos = critPosFalse;
        period = periodFalse;
    } else {
        critPos = critPosTrue;
        period = periodTrue;
    }

    leftStrCmp = cvr_fixed_take_until(pPattern, critPos);
    rightStrCmp = cvr_fixed_slice_between(pPattern, period, period + critPos);

    if (cvr_fixed_eq(&leftStrCmp, &rightStrCmp)) {
        reverse_maximal_suffix(pPattern, period, CVRS_FALSE, &tRevIdxFalse);
        reverse_maximal_suffix(pPattern, period, CVRS_TRUE, &tRevIdxTrue);
        
        critPosBack = pPattern->len - CVRW_MAX(tRevIdxFalse, tRevIdxTrue);

        while (i < pPattern->len - 1) {
            byteset |= (uint64_t)1 << (pattern[i] & 0x3f);
            i++;
        }

        outSearcher.period = period;
        outSearcher.critPosBack = critPosBack;
        outSearcher.byteset = byteset;
        outSearcher.memory = 0;
        outSearcher.memoryBack = pPattern->len;
    } else {
        while (i < pPattern->len) {
            byteset |= (uint64_t)1 << (pattern[i] & 0x3f);
            i++;
        }

        outSearcher.period = CVRW_MAX(critPos, pPattern->len - critPos) + 1;
        outSearcher.critPos = critPos;
        outSearcher.critPosBack = critPos;
        outSearcher.byteset = byteset;
        outSearcher.memory = (size_t)-1; 
        outSearcher.memoryBack = (size_t)-1; 
    }

    outSearcher.pString = pString;
    outSearcher.pPattern = pPattern;
    outSearcher.pos = 0;
    outSearcher.end = pPattern->len;

    return outSearcher;
}


/*
This is an implementation of the two way search (Crochemore-Perrin) algorithm.
Specifically, it uses the implementation from the rust std library.

More details can be found here:
https://www.researchgate.net/publication/220695974_Text_Algorithms
https://dl.acm.org/doi/pdf/10.1145/116825.116845
https://github.com/rust-lang/rust/blob/91e426a0518d5bb3672330c5238ccda6fd26fb54/library/core/src/str/pattern.rs#L1385

Here, I will explain how it works using a practical example.

Lets take this haystack and needle.
haystack = "something somewhere somehow";
needle = "somewhere";

Their indexes are:
haystack:  s o m e t h i n g   s o m e w h e r e   s o m e h o w
           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
needle:    s o m e w h e r e
           0 1 2 3 4 5 6 7 8

The algorithm is based on right and left searches. This are relative to the needle.
It splits right and left using the critical position (calculated when building
the searcher struct)
needle:   s o m e w h | e r e
index:    0 1 2 3 4 5 | 6 7 8
u:        "somewh"              <- left side check
v:                     "ere"    <- right side check
crit_pos: 6
crit position indexes the needle and splits it into u and v (from the paper).

This part (the byteset) is not in the Crochemore-Perrin algorithm.
It is specific to the rust implementation.
The byteset records which byte value occurrs anywhere in the needle.
On the first loop, tailByte is 8 (see _next() function for why). 
Position 8 in the haystack is 'g'.
Since 'g' is not contained anywhere in the needle,
this skips 8 positions. On loop 2, the position is 9.

On loop 2, tailByte is 17, which is the char 'r'.
'r' is contained in the needle, so loop continues.

Right side check.
crit position - index 6 from the needle -> char 'e'
checked against
position + crit position - index 15 from the haystack -> char 'h'
This is not a match, so we move position by one and continue

On loop3, the position is 10 -> char 's' in the haystack
(skip the bytset check)
Right side check.
crit position - index 6 in the needle -> char 'e'
checked agsinst
position + crit position - index 16 from the haystack -> char 'e'
(check the other chars on right side, they all match)

Move on the left side check, from right to left
needle[5] - 'h' == haystack[15] 'h'
needle[4] - 'w' == haystack[14] 'w'
needle[3] - 'e' == haystack[13] 'e'
needle[2] - 'm' == haystack[12] 'm'
needle[1] - 'o' == haystack[11] 'o'
needle[0] - 's' == haystack[10] 's'

Return match found at position 10
*/
CVRS_API int cvr_str_searcher_next(cvr_str_searcher *pSearch, cvr_search_match *pOutMatch)
{
    size_t needleLast = pSearch->pPattern->len - 1; /* needle length, adjusted for indexing */
    char tailByte; /* Always the char at (pos + needleLast)*/
    size_t getChar, start; /* Temporary values */
    size_t i = 0;
    size_t matchPos; /* Pos of final match, if any */

    int skipOuter = 0; /* need to skip the outer loop */
    
    const char* string = (const char*)pSearch->pString->pData;
    const char* pattern = (const char*)pSearch->pPattern->pData;

    char a, b; /* temporary left and right inside the loop */

    for (;;) {
        getChar = pSearch->pos + needleLast;
        if (getChar >= pSearch->pString->len) {
            /* We reached the end of the waystack */
            pSearch->pos = pSearch->pString->len;
            return -1;
        } else {
            tailByte = string[getChar];
        }

        /* Searches if the char at tailByte is contained in the bytset */
        if (((pSearch->byteset >> (tailByte & 0x3f)) & (uint64_t)1) == 0) {
            pSearch->pos += pSearch->pPattern->len;
            continue;
        }

        /* See if the right part of the pattern matches */
        start = pSearch->critPos;
        for (i = start; i < pSearch->pPattern->len; i++) {
            a = pattern[i];
            b = string[pSearch->pos + i];
            if (a != b) {
                pSearch->pos += i - pSearch->critPos + 1;
                skipOuter = 1;
                break;
            }
        }

        if (skipOuter) {
            skipOuter = 0;
            continue;
        }

        /* See if the left part of the needle matches */
        for (i = pSearch->critPos; i > 0; ) {
            i--;
            a = pattern[i];
            b = string[pSearch->pos + i];
            if (a != b) {
                pSearch->pos += pSearch->period;
                skipOuter = 1;
                break;
            }
        }

        if (skipOuter) {
            skipOuter = 0;
            continue;
        }

        /* We have found a match */
        matchPos = pSearch->pos;
        pSearch->pos += pSearch->pPattern->len;

        pOutMatch->string.pData = (void*)((char*)pSearch->pString->pData + matchPos);
        pOutMatch->string.len = pSearch->pos - matchPos;
        pOutMatch->start = matchPos;
        pOutMatch->end = matchPos + pOutMatch->string.len;
        return 0;
    }
}

CVRS_API int cvr_str_searcher_find_first(const cvr_fixed_str *pString, const cvr_fixed_str *pPattern, cvr_search_match *pOutMatch)
{
    cvr_str_searcher twoWaySearcher = { 0 };
    int result;
    
    if (pString == NULL || pPattern == NULL) {
        return -1;
    }

    twoWaySearcher = cvr_str_searcher_new_search(pString, pPattern);
    result = cvr_str_searcher_next(&twoWaySearcher, pOutMatch);
    if (result == -1) {
        return -1;
    }

    return 0;
}

#endif /* CVR_S_IMPLEMENTATION */
