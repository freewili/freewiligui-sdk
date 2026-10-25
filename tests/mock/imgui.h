#ifndef TEST_IMGUI_H
#define TEST_IMGUI_H
#include <stddef.h>
#define IMGUI_VERSION "mock"
#define IMGUI_VERSION_NUM 19280
struct ImGuiContext {};
struct ImGuiIO { int value; };
struct ImDrawVert { float x; };
struct ImVec2 {
    float x, y;
    ImVec2(float x_value = 0, float y_value = 0) : x(x_value), y(y_value) {}
};
typedef void* ImTextureID;
typedef void* (*ImGuiMemAllocFunc)(size_t, void*);
typedef void (*ImGuiMemFreeFunc)(void*, void*);
namespace ImGui {
void SetAllocatorFunctions(ImGuiMemAllocFunc, ImGuiMemFreeFunc, void*);
void SetCurrentContext(ImGuiContext*);
void Image(ImTextureID, const ImVec2&);
}
#endif
