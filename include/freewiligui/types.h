#ifndef FREEWILIGUI_TYPES_H
#define FREEWILIGUI_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <freewiligui/version.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#  define FWGUI_EXTERN_C extern "C"
#else
#  define FWGUI_EXTERN_C extern
#endif
#if defined(_WIN32)
#  define FWGUI_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#  define FWGUI_EXPORT __attribute__((visibility("default")))
#else
#  define FWGUI_EXPORT
#endif
#if defined(FWGUI_PLUGIN_STATIC)
#  define FWGUI_PLUGIN_API FWGUI_EXTERN_C
#else
#  define FWGUI_PLUGIN_API FWGUI_EXTERN_C FWGUI_EXPORT
#endif

#define FWGUI_CONCAT_(a, b) a##b
#define FWGUI_CONCAT(a, b) FWGUI_CONCAT_(a, b)

typedef struct FwGuiHost FwGuiHost;
typedef struct FwGuiPlugin FwGuiPlugin;

typedef struct FwGuiStrView {
    const char* data;
    int32_t len;
} FwGuiStrView;

#define FWGUI_STRVIEW_EMPTY { NULL, 0 }
#define FWGUI_STRVIEW_IS_EMPTY(v) ((v).data == NULL || (v).len <= 0)

typedef int32_t FwGuiResult;
enum FwGuiResultCode {
    FWGUI_OK = 0,
    FWGUI_ERR_FAILED = -1,
    FWGUI_ERR_INVALID_ARG = -2,
    FWGUI_ERR_UNSUPPORTED = -3,
    FWGUI_ERR_NOT_FOUND = -4,
    FWGUI_ERR_STALE_REF = -5,
    FWGUI_ERR_NO_PROJECT = -6,
    FWGUI_ERR_NOT_RUNNING = -7,
    FWGUI_ERR_BUSY = -8,
    FWGUI_ERR_DENIED = -9,
    FWGUI_ERR_NO_MEMORY = -10,
    FWGUI_ERR_WRONG_THREAD = -11,
    FWGUI_ERR_ALREADY = -12,
    FWGUI_ERR_ABI_MISMATCH = -13,
    FWGUI_ERR_INTERNAL = -14,
    FWGUI_ERR_FULL = -15
};

typedef int32_t FwGuiLogLevel;
enum FwGuiLogLevelCode {
    FWGUI_LOG_DEBUG = 0,
    FWGUI_LOG_INFO = 1,
    FWGUI_LOG_WARN = 2,
    FWGUI_LOG_ERROR = 3
};

#define FWGUI_HAS(table_ptr, field)                                           \
    ((table_ptr) != NULL &&                                                   \
     (uint32_t)((table_ptr)->struct_size) >=                                  \
       (uint32_t)((const char*)&((table_ptr)->field) -                         \
                  (const char*)(table_ptr)) +                                 \
       (uint32_t)sizeof((table_ptr)->field))

#ifdef __cplusplus
}
#endif
#endif
