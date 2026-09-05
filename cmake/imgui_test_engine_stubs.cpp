#ifdef IMGUI_ENABLE_TEST_ENGINE
#include <imgui.h>
#include <imgui_internal.h>
extern void ImGuiTestEngineHook_ItemAdd(ImGuiContext*, ImGuiID, const ImRect&,
                                        const ImGuiLastItemData*);
extern void ImGuiTestEngineHook_ItemInfo(ImGuiContext*, ImGuiID, const char*,
                                         ImGuiItemStatusFlags);
extern void ImGuiTestEngineHook_Log(ImGuiContext*, const char*, ...);
extern const char* ImGuiTestEngine_FindItemDebugLabel(ImGuiContext*, ImGuiID);
void ImGuiTestEngineHook_ItemAdd(ImGuiContext*, ImGuiID, const ImRect&,
                                 const ImGuiLastItemData*) {}
void ImGuiTestEngineHook_ItemInfo(ImGuiContext*, ImGuiID, const char*,
                                  ImGuiItemStatusFlags) {}
void ImGuiTestEngineHook_Log(ImGuiContext*, const char*, ...) {}
const char* ImGuiTestEngine_FindItemDebugLabel(ImGuiContext*, ImGuiID) { return nullptr; }
#endif
