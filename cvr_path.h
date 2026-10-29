
#ifndef CVRP_P
#define CVRP_P

#define CVRP_STRINGIFY(x)     #x
#define CVRP_XSTRINGIFY(x)    CVRP_STRINGIFY(x)

#define CVRP_VERSION_MAJOR      0
#define CVRP_VERSION_MINOR      1
#define CVRP_VERSION_REVISION   0
#define CVRP_VERSION_STRING     CVRP_XSTRINGIFY(CVRP_VERSION_MAJOR) "." CVRP_XSTRINGIFY(CVRP_VERSION_MINOR) "." CVRP_WSTRINGIFY(CVRP_VERSION_REVISION)

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h> /* for stat/_stat*/

#ifndef CVRP_API
#define CVRP_API
#endif

#ifdef _WIN32
#define CVRP_WIN
#include <direct.h> /* for _wgetcwd */
#include <wchar.h>
#endif

#ifdef CVRP_WIN
#define CVRP_SEPARATOR '\\'
#else
#define CVRP_SEPARATOR '/'
#endif

typedef uint8_t cvrp_bool8;
#define CVRP_TRUE   1
#define CVRP_FALSE  0

/* We don't double the capacity on grow. Instead we grow by a fixed size */
#define PATH_CAP_GROW_SIZE   256

/* Underlying type-erased path storage. len measures raw bytes */
typedef struct {
    const void *pData;
    size_t len;
} cvr_buf;

typedef struct cvr_path cvr_path;
typedef struct cvr_path cvr_comp;

/* len measures real char or wchar_t count */
struct cvr_path {
    cvr_buf path; /* rename to buf */
    size_t len;
};

typedef struct cvr_dynamic_path cvr_dynamic_path;

struct cvr_dynamic_path {
    cvr_path path;
    size_t capacity;
};

#ifdef CVRP_WIN
CVRP_API cvr_path cvr_path_from_cstr_w(const wchar_t *pCstr);
CVRP_API cvr_path cvr_path_from_parts_w(const wchar_t *pData, size_t len);
#else
CVRP_API cvr_path cvr_path_from_cstr(const char *pCstr);
CVRP_API cvr_path cvr_path_from_parts(const char *pData, size_t len);
#endif
CVRP_API int cvr_path_file_name(const cvr_path *pPath, cvr_path *fileName);
CVRP_API int cvr_path_file_stem(const cvr_path *pPath, cvr_path *fileStem);
CVRP_API int cvr_path_extension(const cvr_path *pPath, cvr_path *extension);

/* TODO: Add when cvr_dynamic_path is ready
CVRP_API int cvr_path_add_ext(const cvr_path *pPath, const cvr_path *pExt);
CVRP_API int cvr_path_change_ext(const cvr_path *pPath, const cvr_path *pExt);
*/

CVRP_API cvrp_bool8 cvr_path_exists(const cvr_path *pPath);
CVRP_API int cvr_path_cwd(cvr_path *output);

CVRP_API cvrp_bool8 cvr_path_eq(const cvr_path *pPath, const cvr_path *pOtherPath);
CVRP_API cvrp_bool8 cvr_path_eq_wchar(const cvr_path *pPath, const wchar_t *pOtherPath);
CVRP_API cvrp_bool8 cvr_path_eq_cstr(const cvr_path *pPath, const char *pOtherPath);

CVRP_API int cvr_path_printf(const cvr_path *pPath);
CVRP_API int cvr_path_printfln(const cvr_path *pPath);
CVRP_API int cvr_path_fprintf(const cvr_path *pPath, FILE *pFile);
CVRP_API int cvr_path_fprintfln(const cvr_path *pPath, FILE *pFile);

/* cvr_dynamic_path functions */

#ifdef CVRP_WIN
CVRP_API int cvr_dynamic_path_from_cstr_init_w(const wchar_t *pCstr, cvr_dynamic_path *pOutput);
CVRP_API int cvr_dynamic_path_from_parts_init_w(const wchar_t *pData, size_t len, cvr_dynamic_path *Output);
#else
CVRP_API int cvr_dynamic_path_from_cstr_init(const char *pCstr, cvr_dynamic_path *pOutput);
CVRP_API int cvr_dynamic_path_from_parts_init(const char *pData, size_t len, cvr_dynamic_path *pOutput);
#endif
CVRP_API void cvr_dynamic_path_uninit(cvr_dynamic_path *pPath);
#endif /* CVRP_H */

typedef struct {
    cvr_path *path;
    size_t pathCount;
    cvrp_bool8 recursive;
    size_t maxDepth;
    cvrp_walker_vtable vtable;
} cvrp_walker_config;


typedef struct {

} cvrp_walker_vtable;

#ifdef CVRP_IMPLEMENTATION

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

static void cvr_path_from_parts_internal(const cvr_path *pPath, cvr_path *fileName, size_t start, size_t end)
{
    size_t len = end - start;

    #ifdef CVRP_WIN
    assert(len % 2 == 0);
    fileName->path.pData = (const char*)pPath->path.pData + start;
    fileName->len = len / 2;
    fileName->path.len = len;
    #else
    fileName->path.pData = (const char*)pPath->path.pData + start;
    fileName->len = len;
    fileName->path.len = len;
    #endif
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

/* Input should be either char or wchar_t */
static cvr_buf cvr_buf_from_parts(const void *pData, size_t len)
{
    cvr_buf path;
    #ifdef CVRP_WIN
    path.len = 2 * len;
    #else
    path.len = len;
    #endif
    path.pData = pData;
    return path;
}

#ifdef CVRP_WIN
CVRP_API cvr_path cvr_path_from_cstr_w(const wchar_t *pCstr)
{
    return cvr_path_from_parts_w(pCstr, wcslen(pCstr));
}

CVRP_API cvr_path cvr_path_from_parts_w(const wchar_t *pData, size_t len)
{
    cvr_path path;
    cvr_buf buf = cvr_buf_from_parts((const void*)pData, len);

    path.len = len; /* We are counting raw bytes here */
    path.path = buf;
    return path;
}
#else
CVRP_API cvr_path cvr_path_from_cstr(const char *pCstr)
{
    return cvr_path_from_parts(pCstr, strlen(pCstr));
}

CVRP_API cvr_path cvr_path_from_parts(const char *pData, size_t len)
{
    cvr_path path;
    cvr_buf buf = cvr_buf_from_parts((const void*)pData, len);

    path.len = len;
    path.path = buf;
    return path;
}
#endif

CVRP_API int cvr_path_file_name(const cvr_path *pPath, cvr_path *fileName)
{
    size_t i;
    size_t end, start, finalLen;
    const char* path = (const char*)(pPath->path.pData);

    if (pPath == NULL || pPath->len == 0) {
        return -1;
    }

    /* Remove any extra path separators or '.' from the start. */
    for (i = pPath->path.len; i > 0; ) {
        i--;
        if (path[i] != '\0' && path[i] != CVRP_SEPARATOR && path[i] != '.') {
            #ifndef CVRP_WIN
            i++; /* We must include the current byte we stopped at */
            #else
            i += 2;
            #endif
            break;
        }
    }

    if (i == 0) {
        return -1;
    }
    end = i;

    /* Find the last component. Search until we reach the separator */
    for (i; i > 0; ) {
        i--;
        if (path[i] == CVRP_SEPARATOR) {
            #ifdef CVRP_WIN /* We must include the current byte we stopped at */
            i += 2;
            #else
            i++;
            #endif
            break;
        }
    }
    start = i;
    finalLen = end - start;
    
    cvr_path_from_parts_internal(pPath, fileName, start, end);
    return 0;
}

CVRP_API int cvr_path_file_stem(const cvr_path *pPath, cvr_path *fileStem)
{
    cvr_path tmp = {0};
    const char* path;
    int result, i;

    if (pPath == NULL || pPath->len == 0) {
        return -1;
    }

    result = cvr_path_file_name(pPath, &tmp);
    if (result == -1) {
        return -1;
    }
    path = (const char*)(tmp.path.pData);

    for (i = 0; i < tmp.path.len; i++) {
        if (path[i] == '.') {
            break;
        }
    }

    cvr_path_from_parts_internal(&tmp, fileStem, 0, i);
    return 0;
}

CVRP_API int cvr_path_extension(const cvr_path *pPath, cvr_path *extension)
{
    cvr_path tmp = {0};
    const char* path;
    int result;
    size_t i;

    if (pPath == NULL || pPath->len == 0) {
        return -1;
    }

    result = cvr_path_file_name(pPath, &tmp);
    if (result == -1) {
        return -1;
    }
    path = (const char*)(tmp.path.pData);

    for (i = tmp.path.len; i > 0; ) {
        i--;
        if (path[i] == '.') {
            #ifdef CVRP_WIN
            i += 2;
            #else
            i++;
            #endif
            break;
        }
        /* 
        If the entire path doesn't contain a '.'
        or we reached a separator without finding a '.', we return an error
        */
        if (path[i] == CVRP_SEPARATOR || i == 0) {
            return -1;
        }
    }

    cvr_path_from_parts_internal(&tmp, extension, i, tmp.path.len);
    return 0;
}

#ifdef CVRP_WIN
CVRP_API cvrp_bool8 cvr_path_exists(const cvr_path *pPath)
{
    struct _stat info;
    wchar_t *path;
    void *pData;
    int result;

    if (pPath == NULL || pPath->len == 0) {
        return CVRP_FALSE;
    }

    pData = malloc(pPath->path.len + 2);
    if (pData == NULL) {
        return CVRP_FALSE;
    }

    memcpy(pData, pPath->path.pData, pPath->path.len);
    *((wchar_t*)pData + pPath->len) = L'\0';

    result = _wstat((wchar_t*)&pData, &info);
    if (result == 1) {
        free(pData);
        return CVRP_FALSE;
    }

    free(pData);
    return CVRP_TRUE;
}

/*
CVRP_API int cvr_path_cwd(cvr_path *output)
{
    wchar_t tmp[1024];
    
    if (_wgetcwd(&tmp, (int)(sizeof(cur) / sizeof(tmp[0]))) == NULL) {
        return -1;
    }
    TODO: wcslen
}
*/
#else
CVRP_API cvrp_bool8 cvr_path_exists(const cvr_path *pPath)
{
    struct stat info;
    char *path;
    void *pData;
    int result;

    if (pPath == NULL || pPath->len == 0) {
        return CVRP_FALSE;
    }
    
    pData = malloc(pPath->path.len + 1);
    if (pData == NULL) {
        return CVRP_FALSE;
    }

    memcpy(pData, pPath->path.pData, pPath->path.len);
    *((char*)pData + pPath->len) = '\0';

    result = stat((char*)&pData, &info);
    if (result == 1) {
        free(pData);
        return CVRP_FALSE;
    }

    free(pData);
    return CVRP_TRUE;
}

/*
CVRP_API int cvr_path_cwd(cvr_path *output)
{
    wchar_t tmp[1024];

    if (getcwd(&cur, sizeof(cur)) == NULL) {
        return -1;
    }
}
*/
#endif

CVRP_API cvrp_bool8 cvr_path_eq(const cvr_path *pPath, const cvr_path *pOtherPath)
{
    int result;

    if (pPath == NULL || pOtherPath == NULL) {
        return CVRP_FALSE;
    }

    if (pPath->path.len != pOtherPath->path.len) {
        return CVRP_FALSE;
    } else {
        return memcmp(pPath->path.pData, pOtherPath->path.pData, pPath->path.len) == 0 ? CVRP_TRUE : CVRP_FALSE;
    }
}

CVRP_API int cvr_path_printf(const cvr_path *pPath)
{
    if (pPath == NULL) {
        return -1;
    }

    return cvr_path_fprintf(pPath, stdout);
}

CVRP_API int cvr_path_printfln(const cvr_path *pPath)
{
    if (pPath == NULL) {
        return -1;
    }

    return cvr_path_fprintfln(pPath, stdout);
}

CVRP_API int cvr_path_fprintf(const cvr_path *pPath, FILE *pFile)
{
    if (pPath == NULL) {
        return -1;
    }

    return write_all_to_fd((char*)pPath->path.pData, pPath->path.len, pFile);
}

CVRP_API int cvr_path_fprintfln(const cvr_path *pPath, FILE *pFile)
{
    char* tBuffer;
    int result;
    size_t len;

    if (pPath == NULL) {
        return -1;
    }
    len = pPath->path.len;

    tBuffer = (char*)malloc(len + 1);

    if (pPath->path.len > 0) {
         memcpy(tBuffer, (char*)pPath->path.pData, len);
    }
    tBuffer[len] = '\n';

    result = write_all_to_fd(tBuffer, len + 1, pFile);
    
    free(tBuffer);

    return result;
}

/* cvr_dynamic_path implementations */

#ifdef CVRP_WIN
CVRP_API int cvr_dynamic_path_from_cstr_init_w(const wchar_t *pPath, cvr_dynamic_path *output)
{
    return cvr_dynamic_path_from_parts_init((void*)pPath, wcslen(pCstr) * sizeof(wchar_t), output);
}
#else
CVRP_API int cvr_dynamic_path_from_cstr_init(const char *pPath, cvr_dynamic_path *pOutput)
{
    return cvr_dynamic_path_from_parts_init((void*)pPath, strlen(pPath), pOutput);
}
#endif

/* len must always count raw bytes */
CVRP_API int cvr_dynamic_path_from_parts_init(const char *pData, size_t len, cvr_dynamic_path *pOutput)
{
    size_t capacity;
    void* pNewData;

    if (pData == NULL) {
        return -1;
    }
    #ifdef CVRP_WIN
    assert(len % sizeof(wchar_t) == 0);
    #endif

    capacity = next_multiple_of(len, PATH_CAP_GROW_SIZE);

    pNewData = malloc(capacity);
    if (pNewData == NULL) {
        return -1;
    }

    memcpy(pNewData, pData, len);

    pOutput->path.path.pData = pNewData;
    pOutput->capacity = capacity;
    #ifdef CVRP_WIN
    pOutput->path.len = len / 2;
    #else
    pOutput->path.len = len;
    #endif
    pOutput->path.path.len = len;

    return 0;
}

CVRP_API void cvr_dynamic_path_uninit(cvr_dynamic_path *pPath)
{
    if (pPath == NULL) {
        return;
    }

    free((void*)pPath->path.path.pData);
}

/* Directory walker implementations */

#endif /* CVRP_IMPLEMENTATION */
