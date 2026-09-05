#ifndef FREEWILIGUI_PLUGIN_H
#define FREEWILIGUI_PLUGIN_H

#include <freewiligui/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct FwGuiHostApi;
typedef struct FwGuiHostApi FwGuiHostApi;

typedef struct FwGuiBuildFingerprint {
    uint32_t abi_version;
    uint32_t imgui_version_num;
    uint32_t implot_version_num;
    uint32_t sizeof_imguiio;
    uint32_t sizeof_imdrawvert;
    uint32_t debug_build;
    uint32_t msc_ver;
    uint32_t crt_dynamic;
    uint32_t imgui_test_engine;
} FwGuiBuildFingerprint;

#if defined(FWGUI_FINGERPRINT_TEST_ENGINE)
#  define FWGUI_FP_TEST_ENGINE ((uint32_t)(FWGUI_FINGERPRINT_TEST_ENGINE))
#elif defined(IMGUI_ENABLE_TEST_ENGINE)
#  define FWGUI_FP_TEST_ENGINE ((uint32_t)1)
#else
#  define FWGUI_FP_TEST_ENGINE ((uint32_t)0)
#endif
#if defined(_DEBUG)
#  define FWGUI_FP_DEBUG_BUILD ((uint32_t)1)
#else
#  define FWGUI_FP_DEBUG_BUILD ((uint32_t)0)
#endif
#if defined(_MSC_VER)
#  define FWGUI_FP_MSC_VER ((uint32_t)_MSC_VER)
#else
#  define FWGUI_FP_MSC_VER ((uint32_t)0)
#endif
#if !defined(_MSC_VER) || defined(_DLL)
#  define FWGUI_FP_CRT_DYNAMIC ((uint32_t)1)
#else
#  define FWGUI_FP_CRT_DYNAMIC ((uint32_t)0)
#endif

#define FWGUI_FINGERPRINT_INIT                 \
    {                                         \
        (uint32_t)FWGUI_ABI_VERSION,           \
        (uint32_t)IMGUI_VERSION_NUM,           \
        (uint32_t)IMPLOT_VERSION_NUM,          \
        (uint32_t)sizeof(ImGuiIO),              \
        (uint32_t)sizeof(ImDrawVert),           \
        FWGUI_FP_DEBUG_BUILD,                   \
        FWGUI_FP_MSC_VER,                       \
        FWGUI_FP_CRT_DYNAMIC,                   \
        FWGUI_FP_TEST_ENGINE                    \
    }

typedef struct FwGuiFrameInfo {
    uint32_t struct_size;
    double dt;
    double host_time;
    uint64_t frame_index;
} FwGuiFrameInfo;

typedef struct FwGuiViewCtx {
    uint32_t struct_size;
    FwGuiHost* host;
    void* user;
    double dt;
    uint64_t frame_index;
} FwGuiViewCtx;

typedef uint32_t FwGuiViewFlags;
enum FwGuiViewFlagBits {
    FWGUI_VIEW_DEFAULT_VISIBLE = 0x00000001u,
    FWGUI_VIEW_MENUBAR = 0x00000008u,
    FWGUI_VIEW_OWNS_WINDOW = 0x00000010u
};

typedef struct FwGuiViewDesc {
    uint32_t struct_size;
    const char* id;
    const char* title;
    const char* menu_group;
    FwGuiViewFlags flags;
    int32_t imgui_window_flags;
    void (*draw)(FwGuiPlugin* plugin, const FwGuiViewCtx* ctx);
    void* user;
    const char* menu_path;
} FwGuiViewDesc;

typedef struct FwGuiDocDesc {
    uint32_t struct_size;
    const char* id;
    const char* title;
    const char* markdown;
} FwGuiDocDesc;

typedef int32_t FwGuiEvent;
enum FwGuiEventCode {
    FWGUI_EVENT_PROJECT_LOADING = 0,
    FWGUI_EVENT_PROJECT_LOADED = 1,
    FWGUI_EVENT_PROJECT_LOAD_FAILED = 2,
    FWGUI_EVENT_THEME_CHANGED = 5,
    FWGUI_EVENT_SHUTDOWN_PENDING = 7,
    FWGUI_EVENT_PROJECT_SAVING = 8,
    FWGUI_EVENT_PROJECT_SAVED = 9,
    FWGUI_EVENT_PROJECT_SAVE_FAILED = 10,
    FWGUI_EVENT_VIEW_VISIBILITY_CHANGED = 11
};

typedef struct FwGuiEventData {
    uint32_t struct_size;
    int32_t code;
    FwGuiStrView text;
} FwGuiEventData;

typedef struct FwGuiPluginDesc {
    uint32_t struct_size;
    const char* id;
    const char* name;
    const char* version;
    const char* author;
    const char* description;
    const char* homepage;
    FwGuiBuildFingerprint build;
} FwGuiPluginDesc;

typedef struct FwGuiPluginVTable {
    uint32_t struct_size;
    FwGuiResult (*startup)(FwGuiPlugin* plugin, FwGuiHost* host,
                           const FwGuiHostApi* api);
    void (*shutdown)(FwGuiPlugin* plugin);
    void (*frame)(FwGuiPlugin* plugin, const FwGuiFrameInfo* info);
    void (*event)(FwGuiPlugin* plugin, FwGuiEvent event,
                  const FwGuiEventData* data);
} FwGuiPluginVTable;

typedef const FwGuiPluginDesc* (*FwGuiPluginQueryFn)(uint32_t host_abi);
typedef FwGuiPlugin* (*FwGuiPluginCreateFn)(const FwGuiPluginVTable** out_vtable);

#if defined(FWGUI_PLUGIN_SHARED)
FWGUI_PLUGIN_API const FwGuiPluginDesc* fwguiPluginQuery(uint32_t host_abi);
FWGUI_PLUGIN_API FwGuiPlugin* fwguiPluginCreate(const FwGuiPluginVTable** out_vtable);
#endif

#if defined(FWGUI_PLUGIN_SHARED) && defined(FWGUI_PLUGIN_STATIC)
#  error "Define exactly one plugin linkage mode."
#endif

#if defined(FWGUI_PLUGIN_STATIC)
#  define FWGUI_PLUGIN_DEFINE_EX(symbol_prefix, desc_ptr, vtable_ptr, create_fn) \
    FWGUI_EXTERN_C const FwGuiPluginDesc* FWGUI_CONCAT(symbol_prefix, _Query)(uint32_t host_abi) { \
        return host_abi == (uint32_t)FWGUI_ABI_VERSION ? (desc_ptr) : NULL; \
    } \
    FWGUI_EXTERN_C FwGuiPlugin* FWGUI_CONCAT(symbol_prefix, _Create)(const FwGuiPluginVTable** out_vtable) { \
        FwGuiPlugin* instance_; \
        if (out_vtable == NULL) return NULL; \
        instance_ = (create_fn)(); \
        if (instance_ == NULL) return NULL; \
        *out_vtable = (vtable_ptr); \
        return instance_; \
    }
#else
#  define FWGUI_PLUGIN_DEFINE_EX(symbol_prefix, desc_ptr, vtable_ptr, create_fn) \
    FWGUI_PLUGIN_API const FwGuiPluginDesc* fwguiPluginQuery(uint32_t host_abi) { \
        return host_abi == (uint32_t)FWGUI_ABI_VERSION ? (desc_ptr) : NULL; \
    } \
    FWGUI_PLUGIN_API FwGuiPlugin* fwguiPluginCreate(const FwGuiPluginVTable** out_vtable) { \
        FwGuiPlugin* instance_; \
        if (out_vtable == NULL) return NULL; \
        instance_ = (create_fn)(); \
        if (instance_ == NULL) return NULL; \
        *out_vtable = (vtable_ptr); \
        return instance_; \
    }
#endif

#define FWGUI_PLUGIN_SINGLETON_FACTORY(symbol_prefix) \
    static FwGuiPlugin* FWGUI_CONCAT(symbol_prefix, _fwgui_singleton_)(void) { \
        static char storage_; \
        return (FwGuiPlugin*)&storage_; \
    }
#define FWGUI_PLUGIN_DEFINE(symbol_prefix, desc_ptr, vtable_ptr) \
    FWGUI_PLUGIN_SINGLETON_FACTORY(symbol_prefix) \
    FWGUI_PLUGIN_DEFINE_EX(symbol_prefix, desc_ptr, vtable_ptr, \
                           FWGUI_CONCAT(symbol_prefix, _fwgui_singleton_))

#ifdef __cplusplus
}
#endif
#endif
