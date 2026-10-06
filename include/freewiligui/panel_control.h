#ifndef FREEWILIGUI_PANEL_CONTROL_H
#define FREEWILIGUI_PANEL_CONTROL_H

#include <freewiligui/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FwGuiHost, FwGuiPlugin and FwGuiCanvas are declared in types.h. paint()
 * receives a borrowed FwGuiCanvas valid only during the call. */
/* One placed control instance, owned by the host. */
typedef struct FwGuiControl FwGuiControl;

typedef int32_t FwGuiPropertyKind;
enum FwGuiPropertyKindCode {
    FWGUI_PROP_BOOL = 1,
    FWGUI_PROP_INT = 2,
    FWGUI_PROP_NUMBER = 3,
    FWGUI_PROP_STRING = 4,
    FWGUI_PROP_COLOR = 5,
    FWGUI_PROP_LIST = 6,
    FWGUI_PROP_FILE = 7,
    FWGUI_PROP_SIGNAL_READ = 8,  /* the host's signal/expression picker (read) */
    FWGUI_PROP_SIGNAL_WRITE = 9  /* the host's signal/expression picker (write) */
};

typedef struct FwGuiPropertyDesc {
    uint32_t struct_size;
    const char* name;          /* [A-Za-z0-9_], 1..64 chars, unique ignoring case */
    FwGuiPropertyKind kind;
    const char* default_value; /* text: "1", "42.5", "#ff8800", "Low", "" */
    double min, max;           /* INT/NUMBER only; min == max means unbounded */
    const char* list_items;    /* LIST only: comma-separated labels */
    const char* help;          /* optional one-line help shown in the grid */
} FwGuiPropertyDesc;

typedef uint32_t FwGuiPaintFlags;
enum FwGuiPaintFlagBits {
    FWGUI_PAINT_EDITING = 0x1u, /* the panel is unlocked (design mode) */
    FWGUI_PAINT_FOCUSED = 0x2u,
    FWGUI_PAINT_ENABLED = 0x4u,
    FWGUI_PAINT_HOVERED = 0x8u
};

typedef struct FwGuiPaintInfo {
    uint32_t struct_size;
    double width, height;      /* control size in panel units; (0,0) is top-left */
    double time, dt;           /* host seconds; dt since this instance's last paint */
    FwGuiPaintFlags flags;
} FwGuiPaintInfo;

typedef int32_t FwGuiInputKind;
enum FwGuiInputKindCode {
    FWGUI_INPUT_MOUSE_DOWN = 1,
    FWGUI_INPUT_MOUSE_UP = 2,
    FWGUI_INPUT_MOUSE_MOVE = 3,
    FWGUI_INPUT_WHEEL = 4,
    FWGUI_INPUT_KEY_DOWN = 5,
    FWGUI_INPUT_KEY_UP = 6,
    FWGUI_INPUT_CHAR = 7,
    FWGUI_INPUT_FOCUS_GAINED = 8,
    FWGUI_INPUT_FOCUS_LOST = 9
};

enum FwGuiModifierBits {
    FWGUI_MOD_SHIFT = 0x1u, FWGUI_MOD_CTRL = 0x2u, FWGUI_MOD_ALT = 0x4u, FWGUI_MOD_SUPER = 0x8u
};
enum FwGuiMouseButtonBits {
    FWGUI_BUTTON_LEFT = 0x1u, FWGUI_BUTTON_RIGHT = 0x2u, FWGUI_BUTTON_MIDDLE = 0x4u
};

/* Key codes are Windows virtual-key values on every platform. Letters and
 * digits use their ASCII upper-case/digit values; text arrives as CHAR. */
enum FwGuiKeyCode {
    FWGUI_KEY_NONE = 0,
    FWGUI_KEY_BACKSPACE = 0x08, FWGUI_KEY_TAB = 0x09, FWGUI_KEY_ENTER = 0x0D,
    FWGUI_KEY_ESCAPE = 0x1B, FWGUI_KEY_SPACE = 0x20,
    FWGUI_KEY_PAGE_UP = 0x21, FWGUI_KEY_PAGE_DOWN = 0x22,
    FWGUI_KEY_END = 0x23, FWGUI_KEY_HOME = 0x24,
    FWGUI_KEY_LEFT = 0x25, FWGUI_KEY_UP = 0x26, FWGUI_KEY_RIGHT = 0x27, FWGUI_KEY_DOWN = 0x28,
    FWGUI_KEY_INSERT = 0x2D, FWGUI_KEY_DELETE = 0x2E
};

typedef struct FwGuiInput {
    uint32_t struct_size;
    FwGuiInputKind kind;
    double x, y;               /* control-local panel units */
    uint32_t buttons;          /* FwGuiMouseButtonBits held */
    uint32_t mods;             /* FwGuiModifierBits held */
    double wheel;              /* WHEEL: positive = up */
    int32_t key;               /* KEY_DOWN/KEY_UP: FwGuiKeyCode */
    uint32_t codepoint;        /* CHAR: UTF-32 */
} FwGuiInput;

typedef struct FwGuiControlCallbacks {
    uint32_t struct_size;
    /* Returns the plugin's per-instance state (may be NULL). */
    void* (*create)(void* type_user, FwGuiControl* control);
    void (*destroy)(void* state);
    void (*paint)(void* state, FwGuiCanvas* target, const FwGuiPaintInfo* info);
    /* Optional. Return 1 when the input was used. */
    int (*input)(void* state, const FwGuiInput* input);
    /* Optional. Called after the host stores a new value. */
    void (*property_changed)(void* state, const char* name);
} FwGuiControlCallbacks;

typedef struct FwGuiControlTypeDesc {
    uint32_t struct_size;
    const char* id;                /* <= 256 chars; no control chars, '=' or '/' */
    const char* label;             /* palette + grid display name */
    const char* palette_section;   /* a built-in section name, or NULL for "Plugins" */
    const void* icon_png;          /* optional PNG, <= 64x64 */
    uint32_t icon_png_size;
    double default_width, default_height;
    const FwGuiPropertyDesc* properties;
    uint32_t property_count;       /* <= 32 */
    FwGuiControlCallbacks callbacks;
    void* type_user;
} FwGuiControlTypeDesc;

typedef struct FwGuiPanelControlApi {
    uint32_t struct_size;
    FwGuiResult (*register_type)(FwGuiHost*, FwGuiPlugin*, const FwGuiControlTypeDesc*);
    FwGuiResult (*unregister_type)(FwGuiHost*, FwGuiPlugin*, const char* id);
    /* Instance helpers: UI thread only. */
    double (*get_number)(FwGuiHost*, FwGuiControl*, const char* name);
    /* Valid until the current callback returns (or the next helper call). */
    FwGuiStrView (*get_string)(FwGuiHost*, FwGuiControl*, const char* name);
    FwGuiResult (*set_property)(FwGuiHost*, FwGuiControl*, const char* name, const char* value);
    FwGuiResult (*read_signal)(FwGuiHost*, FwGuiControl*, const char* name,
                               double* out_number, FwGuiStrView* out_text, int32_t* out_is_text);
    FwGuiResult (*write_signal)(FwGuiHost*, FwGuiControl*, const char* name,
                                double number, const char* text_or_null);
    FwGuiResult (*request_focus)(FwGuiHost*, FwGuiControl*);
    FwGuiResult (*release_focus)(FwGuiHost*, FwGuiControl*);
} FwGuiPanelControlApi;

#ifdef __cplusplus
}
#endif
#endif
