#ifndef FREEWILIGUI_PLUGIN_HPP
#define FREEWILIGUI_PLUGIN_HPP

#include <imgui.h>
#include <implot.h>
#include <freewiligui/host.hpp>

namespace fwgui {
namespace detail {
struct ImGuiBinder {
    static void Bind(const FwGuiUiApi* ui, FwGuiHost* h) noexcept {
        if (!ui) return;
        if (FWGUI_HAS(ui, get_allocator_functions) && ui->get_allocator_functions) {
            FwGuiAllocFn af = nullptr; FwGuiFreeFn ff = nullptr; void* u = nullptr;
            ui->get_allocator_functions(h, &af, &ff, &u);
            if (af && ff) ImGui::SetAllocatorFunctions(af, ff, u);
        }
        if (FWGUI_HAS(ui, imgui_context) && ui->imgui_context)
            ImGui::SetCurrentContext(static_cast<ImGuiContext*>(ui->imgui_context(h)));
        if (FWGUI_HAS(ui, implot_context) && ui->implot_context)
            ImPlot::SetCurrentContext(static_cast<ImPlotContext*>(ui->implot_context(h)));
    }
};
}
template<class Derived> using Plugin = PluginBase<Derived, detail::ImGuiBinder>;
}

#define FWGUI_PLUGIN_MAIN(symbol_prefix, desc_ptr, plugin_type) \
    FWGUI_PLUGIN_DEFINE_EX(symbol_prefix, desc_ptr, plugin_type::VTable(), plugin_type::Create)

#endif