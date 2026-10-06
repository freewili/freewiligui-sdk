#include <freewiligui/host_api.h>

static FwGuiResult startup(FwGuiPlugin* plugin, FwGuiHost* host,
                           const FwGuiHostApi* api) {
    (void)plugin;
    (void)host;
    (void)api;
    return FWGUI_OK;
}
static void shutdown(FwGuiPlugin* plugin) { (void)plugin; }
static const FwGuiPluginVTable table = {
    sizeof(FwGuiPluginVTable), startup, shutdown, NULL, NULL
};
static const FwGuiPluginDesc descriptor = {
    sizeof(FwGuiPluginDesc), "org.freewili.test-c", "C ABI test", "1.0.0",
    "", "", NULL, { FWGUI_ABI_VERSION, 0, 0, 0, 0, 0, 0, 1, 0 }, 0
};
FWGUI_PLUGIN_DEFINE(freewili_c_test, &descriptor, &table)
