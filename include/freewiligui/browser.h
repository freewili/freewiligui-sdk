#ifndef FREEWILIGUI_BROWSER_H
#define FREEWILIGUI_BROWSER_H
#include <freewiligui/types.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
#define FWGUI_BROWSER_CONTRACT_VERSION 1u
#define FWGUI_BROWSER_MAX_PNG_BYTES (16u * 1024u * 1024u)
#define FWGUI_BROWSER_MAX_IMAGE_DIMENSION 4096u

typedef uint32_t FwGuiPluginKinds;
enum FwGuiPluginKindBits {
    FWGUI_PLUGIN_KIND_VIEW = 0x1u,
    FWGUI_PLUGIN_KIND_PANEL_CONTROL = 0x2u,
    FWGUI_PLUGIN_KIND_MCP_TOOL = 0x4u
};
/* Required for SDK 0.3.0+ plugins. All pointers have library lifetime.
 * PNG bytes and UTF-8 Markdown are embedded, not paths or startup callbacks.
 * This descriptor must be available from query, including while disabled. */
typedef struct FwGuiPluginBrowserInfo {
    uint32_t struct_size;
    FwGuiPluginKinds kinds;
    const unsigned char* thumbnail_png;
    uint32_t thumbnail_png_size;
    const char* help_markdown;
} FwGuiPluginBrowserInfo;

static inline int fwguiBrowserHasText(const char* text) {
    if (!text) return 0;
    while (*text) { if (*text != ' ' && *text != '\t' && *text != '\r' && *text != '\n') return 1; ++text; }
    return 0;
}
/* Returns NULL on valid contract data, otherwise a static diagnostic string.
 * Image validation here checks the PNG header and bounds; hosts decode PNG
 * data normally and report a decoding failure separately. */
static inline const char* fwguiBrowserInfoError(const FwGuiPluginBrowserInfo* info) {
    static const unsigned char signature[8] = {137,80,78,71,13,10,26,10};
    const unsigned char* bytes;
    uint32_t w, h;
    if (!info || info->struct_size < offsetof(FwGuiPluginBrowserInfo, help_markdown) + sizeof(info->help_markdown))
        return "Browser metadata is required (FwGuiPluginBrowserInfo)";
    if (!info->kinds || (info->kinds & ~(uint32_t)7u)) return "Plugin kinds must declare views, panel controls or MCP tools";
    if (!fwguiBrowserHasText(info->help_markdown)) return "Plugin help Markdown is required";
    { uint32_t length = 0; while (length <= 1048576u && info->help_markdown[length]) ++length;
      if (length > 1048576u) return "Plugin help Markdown exceeds 1 MiB"; }
    if (!info->thumbnail_png || info->thumbnail_png_size < 33u || info->thumbnail_png_size > FWGUI_BROWSER_MAX_PNG_BYTES)
        return "An embedded PNG thumbnail is required (maximum 16 MiB)";
    bytes = info->thumbnail_png;
    if (memcmp(bytes, signature, 8) || bytes[8] || bytes[9] || bytes[10] || bytes[11] != 13 || memcmp(bytes + 12, "IHDR", 4))
        return "Thumbnail must begin with a valid PNG signature and IHDR";
    w = ((uint32_t)bytes[16] << 24) | ((uint32_t)bytes[17] << 16) | ((uint32_t)bytes[18] << 8) | bytes[19];
    h = ((uint32_t)bytes[20] << 24) | ((uint32_t)bytes[21] << 16) | ((uint32_t)bytes[22] << 8) | bytes[23];
    if (!w || !h || w > FWGUI_BROWSER_MAX_IMAGE_DIMENSION || h > FWGUI_BROWSER_MAX_IMAGE_DIMENSION)
        return "Thumbnail dimensions must be 1..4096 pixels";
    return NULL;
}
#ifdef __cplusplus
}
#endif
#endif
