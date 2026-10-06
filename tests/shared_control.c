/* A complete panel-control plugin in C99 with no ImGui anywhere. */
#include <freewiligui/host_api.h>
#include <stddef.h>

static const FwGuiHostApi* g_api;
static FwGuiHost* g_host;

static void* create(void* type_user, FwGuiControl* control) { (void)type_user; return control; }
static void destroy(void* state) { (void)state; }
static void paint(void* state, FwGuiCanvas* c, const FwGuiPaintInfo* info) {
    const FwGuiCanvasApi* cv = g_api->canvas;
    (void)state;
    if (!cv) return;
    cv->set_fill_style(g_host, c, 0xFF2080FFu);
    cv->fill_rect(g_host, c, 0, 0, info->width, info->height);
}
static const FwGuiPropertyDesc props[] = {
    { sizeof(FwGuiPropertyDesc), "value", FWGUI_PROP_NUMBER, "0", 0, 100, NULL, "Value" }
};
static FwGuiResult startup(FwGuiPlugin* plugin, FwGuiHost* host, const FwGuiHostApi* api) {
    FwGuiControlTypeDesc d;
    g_api = api; g_host = host;
    if (!FWGUI_HAS(api, panel_control) || !api->panel_control) return FWGUI_OK;
    d.struct_size = sizeof d; d.id = "box"; d.label = "C Box"; d.palette_section = NULL;
    d.icon_png = NULL; d.icon_png_size = 0; d.default_width = 80; d.default_height = 40;
    d.properties = props; d.property_count = 1;
    d.callbacks.struct_size = sizeof d.callbacks; d.callbacks.create = create;
    d.callbacks.destroy = destroy; d.callbacks.paint = paint;
    d.callbacks.input = NULL; d.callbacks.property_changed = NULL;
    d.type_user = NULL;
    return api->panel_control->register_type(host, plugin, &d);
}
static void shutdown(FwGuiPlugin* plugin) { (void)plugin; }
static const FwGuiPluginVTable table = { sizeof(FwGuiPluginVTable), startup, shutdown, NULL, NULL };
static const FwGuiPluginDesc descriptor = {
    sizeof(FwGuiPluginDesc), "org.freewili.test-c-control", "C control test", "1.0.0",
    "FreeWili", "C99 panel control", NULL, FWGUI_FINGERPRINT_INIT_NO_IMGUI, FWGUI_PLUGIN_NO_IMGUI
};
FWGUI_PLUGIN_DEFINE(freewili_c_control, &descriptor, &table)
