
#define CVRP_IMPLEMENTATION
#include "./../local_cvr_path.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

void cvr_path_test_init()
{
    cvr_path path1;
    path1 = cvr_path_from_cstr_w(L"/path/main.c");

    assert(path1.len == 12);
}

void cvr_path_test_filename()
{
    cvr_path path1 = {0};
    cvr_path filename1 = {0};
    cvr_path filenameCheck1 = {0};
    int result;
    path1 = cvr_path_from_cstr(L"path\\main.c");
    filenameCheck1 = cvr_path_from_cstr("main.c");

    result = cvr_path_file_name(&path1, &filename1);
    assert(result != -1);

    assert(cvr_path_eq(&filename1, &filenameCheck1));

    cvr_path path2 = {0};
    cvr_path filename2 = {0};
    cvr_path filenameCheck2 = {0};

    path2 = cvr_path_from_cstr(L"path\\main.c\\");
    filenameCheck2 = cvr_path_from_cstr("main.c");

    result = cvr_path_file_name(&path2, &filename2);
    assert(result != -1);

    assert(cvr_path_eq(&filename2, &filenameCheck2));

    cvr_path path3 = {0};
    cvr_path filename3 = {0};
    cvr_path filenameCheck3 = {0};

    path3 = cvr_path_from_cstr("path\\main");
    filenameCheck3 = cvr_path_from_cstr("main");

    result = cvr_path_file_name(&path3, &filename3);
    assert(result != -1);

    assert(cvr_path_eq(&filename3, &filenameCheck3));

    cvr_path path4 = {0};
    cvr_path filename4 = {0};
    cvr_path filenameCheck4 = {0};

    path4 = cvr_path_from_cstr("\\path\\main.c\\.");
    filenameCheck4 = cvr_path_from_cstr("main.c");

    result = cvr_path_file_name(&path4, &filename4);
    assert(result != -1);

    assert(cvr_path_eq(&filename4, &filenameCheck4));

    cvr_path path5 = {0};
    cvr_path filename5 = {0};
    cvr_path filenameCheck5 = {0};

    path5 = cvr_path_from_cstr("path\\main.c\\.\\\\");
    filenameCheck5 = cvr_path_from_cstr("main.c");

    result = cvr_path_file_name(&path5, &filename5);
    assert(result != -1);

    assert(cvr_path_eq(&filename5, &filenameCheck5));

    cvr_path path6 = {0};
    cvr_path filename6 = {0};

    path6 = cvr_path_from_cstr("//");

    result = cvr_path_file_name(&path6, &filename6);
    assert(result == -1);
}

void main()
{
    #ifndef _WIN32
    return;
    #endif

    cvr_path_test_init();

    printf("ALL TESTS PASS\n");
}
