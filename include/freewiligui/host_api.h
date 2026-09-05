#ifndef FREEWILIGUI_HOST_API_H
#define FREEWILIGUI_HOST_API_H

#include <freewiligui/freewili.h>
#include <freewiligui/plugin.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FwGuiCoreApi {
    uint32_t struct_size;
    uint32_t (*abi_version)(FwGuiHost* host);
    FwGuiStrView (*host_version)(FwGuiHost* host);
    FwGuiStrView (*host_name)(FwGuiHost* host);
    void (*log)(FwGuiHost* host, FwGuiPlugin* plugin, FwGuiLogLevel level,
                const char* message);
    FwGuiStrView (*last_error)(FwGuiHost* host);
    int32_t (*is_main_thread)(FwGuiHost* host);
    void (*assert_main_thread)(FwGuiHost* host, const char* where);
    FwGuiStrView (*settings_get)(FwGuiHost* host, FwGuiPlugin* plugin,
                                 const char* key);
    FwGuiResult (*settings_set)(FwGuiHost* host, FwGuiPlugin* plugin,
                                const char* key, const char* value);
    double (*settings_get_number)(FwGuiHost* host, FwGuiPlugin* plugin,
                                  const char* key, double fallback);
    FwGuiResult (*settings_set_number)(FwGuiHost* host, FwGuiPlugin* plugin,
                                       const char* key, double value);
    FwGuiStrView (*data_dir)(FwGuiHost* host);
    FwGuiStrView (*plugin_path)(FwGuiHost* host, FwGuiPlugin* plugin);
    double (*host_time)(FwGuiHost* host);
    FwGuiResult (*settings_erase)(FwGuiHost* host, FwGuiPlugin* plugin,
                                  const char* key);
} FwGuiCoreApi;

typedef void* (*FwGuiAllocFn)(size_t size, void* user_data);
typedef void (*FwGuiFreeFn)(void* ptr, void* user_data);

typedef struct FwGuiUiApi {
    uint32_t struct_size;
    void* (*imgui_context)(FwGuiHost* host);
    void* (*implot_context)(FwGuiHost* host);
    void (*get_allocator_functions)(FwGuiHost* host, FwGuiAllocFn* out_alloc,
                                    FwGuiFreeFn* out_free, void** out_user_data);
    FwGuiResult (*register_view)(FwGuiHost* host, FwGuiPlugin* plugin,
                                 const FwGuiViewDesc* desc);
    FwGuiResult (*unregister_view)(FwGuiHost* host, FwGuiPlugin* plugin,
                                   const char* view_id);
    int32_t (*view_is_visible)(FwGuiHost* host, FwGuiPlugin* plugin,
                               const char* view_id);
    void (*set_view_visible)(FwGuiHost* host, FwGuiPlugin* plugin,
                             const char* view_id, int32_t visible);
    void (*focus_view)(FwGuiHost* host, FwGuiPlugin* plugin, const char* view_id);
    float (*dpi_scale)(FwGuiHost* host);
    FwGuiStrView (*theme_name)(FwGuiHost* host);
    FwGuiResult (*register_doc)(FwGuiHost* host, FwGuiPlugin* plugin,
                                const FwGuiDocDesc* desc);
    void (*open_doc)(FwGuiHost* host, FwGuiPlugin* plugin, const char* doc_id);
    const char* (*tr)(FwGuiHost* host, FwGuiPlugin* plugin, const char* english);
    void (*push_font)(FwGuiHost* host, int32_t kind);
    void (*pop_font)(FwGuiHost* host);
    int32_t (*splitter)(FwGuiHost* host, const char* id, float* size,
                        float extent, float min_before, float min_after,
                        int32_t vertical);
} FwGuiUiApi;

enum FwGuiFontKind {
    FWGUI_FONT_DEFAULT = 0,
    FWGUI_FONT_BOLD = 1,
    FWGUI_FONT_ITALIC = 2,
    FWGUI_FONT_MONO = 3
};

typedef struct FwGuiMcpReply FwGuiMcpReply;
typedef void (*FwGuiMcpToolFn)(FwGuiPlugin* plugin, const char* args_json,
                               FwGuiMcpReply* reply, void* user);

typedef struct FwGuiMcpApi {
    uint32_t struct_size;
    int32_t (*is_enabled)(FwGuiHost* host);
    FwGuiResult (*register_tool)(FwGuiHost* host, FwGuiPlugin* plugin,
                                 const char* name, const char* description,
                                 const char* schema_json,
                                 FwGuiMcpToolFn handler, void* user);
    FwGuiResult (*unregister_tool)(FwGuiHost* host, FwGuiPlugin* plugin,
                                   const char* name);
    void (*reply_ok)(FwGuiMcpReply* reply, const char* content_json);
    void (*reply_text)(FwGuiMcpReply* reply, const char* text);
    void (*reply_error)(FwGuiMcpReply* reply, const char* message);
    FwGuiStrView (*tool_full_name)(FwGuiHost* host, FwGuiPlugin* plugin,
                                   const char* name);
} FwGuiMcpApi;

typedef struct FwGuiCanvas FwGuiCanvas;
typedef struct FwGuiCanvasImage FwGuiCanvasImage;

typedef struct FwGuiCanvasTextMetrics {
    double width;
    double height;
    double ascent;
    double descent;
} FwGuiCanvasTextMetrics;

#define FWGUI_CANVAS_ALIGN_LEFT 0
#define FWGUI_CANVAS_ALIGN_RIGHT 1
#define FWGUI_CANVAS_ALIGN_CENTER 2
#define FWGUI_CANVAS_ALIGN_START 3
#define FWGUI_CANVAS_ALIGN_END 4
#define FWGUI_CANVAS_BASELINE_TOP 0
#define FWGUI_CANVAS_BASELINE_HANGING 1
#define FWGUI_CANVAS_BASELINE_MIDDLE 2
#define FWGUI_CANVAS_BASELINE_ALPHABETIC 3
#define FWGUI_CANVAS_BASELINE_IDEOGRAPHIC 4
#define FWGUI_CANVAS_BASELINE_BOTTOM 5
#define FWGUI_CANVAS_CAP_BUTT 0
#define FWGUI_CANVAS_CAP_ROUND 1
#define FWGUI_CANVAS_CAP_SQUARE 2
#define FWGUI_CANVAS_JOIN_MITER 0
#define FWGUI_CANVAS_JOIN_ROUND 1
#define FWGUI_CANVAS_JOIN_BEVEL 2

typedef struct FwGuiCanvasApi {
    uint32_t struct_size;
    FwGuiCanvas* (*create)(FwGuiHost*, FwGuiPlugin*, int32_t, int32_t);
    void (*destroy)(FwGuiHost*, FwGuiCanvas*);
    void (*resize)(FwGuiHost*, FwGuiCanvas*, int32_t, int32_t);
    int32_t (*width)(FwGuiHost*, FwGuiCanvas*);
    int32_t (*height)(FwGuiHost*, FwGuiCanvas*);
    void (*begin)(FwGuiHost*, FwGuiCanvas*, uint32_t);
    void (*end)(FwGuiHost*, FwGuiCanvas*);
    uint64_t (*texture_id)(FwGuiHost*, FwGuiCanvas*);
    void (*save)(FwGuiHost*, FwGuiCanvas*);
    void (*restore)(FwGuiHost*, FwGuiCanvas*);
    void (*set_global_alpha)(FwGuiHost*, FwGuiCanvas*, double);
    void (*set_fill_style_css)(FwGuiHost*, FwGuiCanvas*, const char*);
    void (*set_fill_style)(FwGuiHost*, FwGuiCanvas*, uint32_t);
    void (*set_fill_style_corners)(FwGuiHost*, FwGuiCanvas*, uint32_t,
                                   uint32_t, uint32_t, uint32_t);
    void (*set_stroke_style_css)(FwGuiHost*, FwGuiCanvas*, const char*);
    void (*set_stroke_style)(FwGuiHost*, FwGuiCanvas*, uint32_t);
    void (*set_line_width)(FwGuiHost*, FwGuiCanvas*, double);
    void (*set_line_cap)(FwGuiHost*, FwGuiCanvas*, int32_t);
    void (*set_line_join)(FwGuiHost*, FwGuiCanvas*, int32_t);
    void (*fill_rect)(FwGuiHost*, FwGuiCanvas*, double, double, double, double);
    void (*stroke_rect)(FwGuiHost*, FwGuiCanvas*, double, double, double, double);
    void (*clear_rect)(FwGuiHost*, FwGuiCanvas*, double, double, double, double);
    void (*fill_stroke_round_rect)(FwGuiHost*, FwGuiCanvas*, double, double,
                                   double, double, double);
    void (*set_clip_rect)(FwGuiHost*, FwGuiCanvas*, double, double, double, double);
    void (*clear_clip)(FwGuiHost*, FwGuiCanvas*);
    void (*begin_path)(FwGuiHost*, FwGuiCanvas*);
    void (*close_path)(FwGuiHost*, FwGuiCanvas*);
    void (*move_to)(FwGuiHost*, FwGuiCanvas*, double, double);
    void (*line_to)(FwGuiHost*, FwGuiCanvas*, double, double);
    void (*rect)(FwGuiHost*, FwGuiCanvas*, double, double, double, double);
    void (*arc)(FwGuiHost*, FwGuiCanvas*, double, double, double, double,
                double, int32_t);
    void (*arc_to)(FwGuiHost*, FwGuiCanvas*, double, double, double, double,
                   double);
    void (*quadratic_curve_to)(FwGuiHost*, FwGuiCanvas*, double, double,
                               double, double);
    void (*bezier_curve_to)(FwGuiHost*, FwGuiCanvas*, double, double, double,
                            double, double, double);
    void (*fill)(FwGuiHost*, FwGuiCanvas*);
    void (*stroke)(FwGuiHost*, FwGuiCanvas*);
    void (*fill_triangle)(FwGuiHost*, FwGuiCanvas*, double, double, double,
                          double, double, double);
    void (*set_font_css)(FwGuiHost*, FwGuiCanvas*, const char*);
    void (*set_font)(FwGuiHost*, FwGuiCanvas*, double, int32_t, int32_t);
    void (*set_text_align)(FwGuiHost*, FwGuiCanvas*, int32_t);
    void (*set_text_baseline)(FwGuiHost*, FwGuiCanvas*, int32_t);
    void (*fill_text)(FwGuiHost*, FwGuiCanvas*, const char*, double, double);
    void (*measure_text)(FwGuiHost*, FwGuiCanvas*, const char*,
                         FwGuiCanvasTextMetrics*);
    FwGuiCanvasImage* (*image_load_file)(FwGuiHost*, FwGuiPlugin*, const char*);
    FwGuiCanvasImage* (*image_load_memory)(FwGuiHost*, FwGuiPlugin*,
                                           const void*, size_t);
    FwGuiCanvasImage* (*image_load_pixels)(FwGuiHost*, FwGuiPlugin*,
                                           const void*, int32_t, int32_t);
    void (*image_destroy)(FwGuiHost*, FwGuiCanvasImage*);
    int32_t (*image_width)(FwGuiHost*, FwGuiCanvasImage*);
    int32_t (*image_height)(FwGuiHost*, FwGuiCanvasImage*);
    void (*draw_image)(FwGuiHost*, FwGuiCanvas*, FwGuiCanvasImage*,
                       double, double);
    void (*draw_image_scaled)(FwGuiHost*, FwGuiCanvas*, FwGuiCanvasImage*,
                              double, double, double, double);
    void (*draw_image_rgba)(FwGuiHost*, FwGuiCanvas*, const void*, int32_t,
                            int32_t, double, double, double, double);
    size_t (*read_pixels)(FwGuiHost*, FwGuiCanvas*, void*, size_t);
    uint32_t (*pixel_at)(FwGuiHost*, FwGuiCanvas*, int32_t, int32_t);
    void (*set_line_dash)(FwGuiHost*, FwGuiCanvas*, const double*, int32_t);
    void (*set_line_dash_offset)(FwGuiHost*, FwGuiCanvas*, double);
    void (*translate)(FwGuiHost*, FwGuiCanvas*, double, double);
    void (*rotate)(FwGuiHost*, FwGuiCanvas*, double);
    void (*scale)(FwGuiHost*, FwGuiCanvas*, double, double);
    void (*set_transform)(FwGuiHost*, FwGuiCanvas*, double, double, double,
                          double, double, double);
    void (*reset_transform)(FwGuiHost*, FwGuiCanvas*);
} FwGuiCanvasApi;

struct FwGuiHostApi {
    uint32_t struct_size;
    uint32_t abi_version;
    const FwGuiCoreApi* core;
    const FwGuiUiApi* ui;
    const FwGuiCanvasApi* canvas;
    const FwGuiMcpApi* mcp;
    const FwGuiFreeWiliApi* freewili;
};

#ifdef __cplusplus
}
#endif
#endif
