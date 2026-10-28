
#ifndef CVRP_P
#define CVRP_P

#define CVRP_STRINGIFY(x)     #x
#define CVRP_XSTRINGIFY(x)    CVRP_STRINGIFY(x)

#define CVRP_VERSION_MAJOR      0
#define CVRP_VERSION_MINOR      1
#define CVRP_VERSION_REVISION   0
#define CVRP_VERSION_STRING     CVRP_XSTRINGIFY(CVRP_VERSION_MAJOR) "." CVRP_XSTRINGIFY(CVRP_VERSION_MINOR) "." CVRP_WSTRINGIFY(CVRP_VERSION_REVISION)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <errno.h>

#ifndef CVRP_API
#define CVRP_API
#endif

#ifdef _WIN32
#define CVRP_WIN
#endif

#ifdef CVRP_WIN
#define CVRP_SEPARATOR '\\'
#else
#define CVRP_SEPARATOR '/'
#endif

typedef uint8_t cvrp_bool8;
#define CVRP_TRUE   1
#define CVRP_FALSE  0

/* Underlying type-erased path storage. len measures raw bytes */
typedef struct {
    const void *pData;
    size_t len;
} cvr_buf;

typedef struct cvr_path cvr_path;
typedef struct cvr_path cvr_comp;

/* len measures real char or wchar_t count */
struct cvr_path {
    cvr_buf path;
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

CVRP_API int cvr_path_exists(const cvr_path *output);
CVRP_API int cvr_path_cwd(cvr_path *output);

CVRP_API cvrp_bool8 cvr_path_eq(const cvr_path *pPath, const cvr_path *pOtherPath);
CVRP_API cvrp_bool8 cvr_path_eq_wchar(const cvr_path *pPath, const wchar_t *pOtherPath);
CVRP_API cvrp_bool8 cvr_path_eq_cstr(const cvr_path *pPath, const char *pOtherPath);

CVRP_API int cvr_path_printf(const cvr_path *pPath);
CVRP_API int cvr_path_printfln(const cvr_path *pPath);
CVRP_API int cvr_path_fprintf(const cvr_path *pPath, FILE *pFile);
CVRP_API int cvr_path_fprintfln(const cvr_path *pPath, FILE *pFile);

#endif /* CVRP_H */

#ifdef CVRP_IMPLEMENTATION

static void cvr_path_from_parts_internal(const cvr_path *pPath, cvr_path *fileName, size_t start, size_t end)
{
    size_t len = end - start;

    #ifdef CVRP_WIN
    assert(finalLen % 2 == 0);
    fileName->path = (const char*)pPath->path->pData + start;
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
    path.len = len;
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

    path.len = len * 2; /* We are counting raw bytes here */
    path.pData = buf;
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
    cvr_buf tmp = {0};

    if (pPath == NULL || pPath->len == 0) {
        return -1;
    }

    /* Remove any extra path separators or '.' from the start. */
    for (i = pPath->path.len; i > 0; ) {
        i--;
        if (path[i] != '\0' && path[i] != CVRP_SEPARATOR && path[i] != '.') {
            i++; /* We must include the current byte we stopped at */
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
            i++;
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
    int result, i;

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
            i++;
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

CVRP_API cvrp_bool8 cvr_path_eq(const cvr_path *pPath, const cvr_path *pOtherPath)
{
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
    int result, len;

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

#endif
