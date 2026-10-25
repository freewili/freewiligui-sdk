#include <freewiligui/plugin.h>
#include "freewiligui_browser.h"
#if defined(LEGACY_BROWSER)
#define DESC_SIZE offsetof(FwGuiPluginDesc,browser)
#else
#define DESC_SIZE sizeof(FwGuiPluginDesc)
#endif
#if defined(OMIT_BROWSER) || defined(LEGACY_BROWSER)
#define BROWSER_DATA NULL
#else
#define BROWSER_DATA FWGUI_BROWSER_INFO
#endif
static const FwGuiPluginDesc descriptor={DESC_SIZE,"org.freewili.browsercontract","Browser contract fixture","1.0","FreeWili","Embedded browser data available without startup",NULL,FWGUI_FINGERPRINT_INIT_NO_IMGUI,FWGUI_PLUGIN_NO_IMGUI,BROWSER_DATA};
FWGUI_EXPORT const FwGuiPluginDesc* fwguiPluginQuery(uint32_t abi) { return abi==FWGUI_ABI_VERSION?&descriptor:NULL; }
FWGUI_EXPORT FwGuiPlugin* fwguiPluginCreate(const FwGuiPluginVTable** out) { if(out)*out=NULL;return NULL; }
