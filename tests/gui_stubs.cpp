#include <imgui.h>
#include <implot.h>
#include "gui_stubs.h"

float freewiligui_test_image_width = 0;
float freewiligui_test_image_height = 0;

namespace ImGui {
void SetAllocatorFunctions(ImGuiMemAllocFunc, ImGuiMemFreeFunc, void*) {}
void SetCurrentContext(ImGuiContext*) {}
void Image(ImTextureID, const ImVec2& size) {
    freewiligui_test_image_width = size.x;
    freewiligui_test_image_height = size.y;
}
}
namespace ImPlot {
void SetCurrentContext(ImPlotContext*) {}
}
