#include <freewiligui/panel_control.hpp>
#include <cassert>
#include <cstring>
#include <string>
#include <utility>
#ifdef IMGUI_VERSION
#error "panel_control.hpp must not pull in ImGui"
#endif

namespace {
FwGuiControlTypeDesc g_registered{};
std::string g_lastSet;
FwGuiResult reg(FwGuiHost*, FwGuiPlugin*, const FwGuiControlTypeDesc* d) { g_registered = *d; return FWGUI_OK; }
double num(FwGuiHost*, FwGuiControl*, const char* n) { return std::strcmp(n, "value") == 0 ? 7.5 : 0; }
FwGuiResult set(FwGuiHost*, FwGuiControl*, const char* n, const char* v) { g_lastSet = std::string(n) + "=" + v; return FWGUI_OK; }
int g_destroyed = 0;
int g_canvasCreated = 0, g_canvasDestroyed = 0;
FwGuiCanvas* canvasCreate(FwGuiHost*, FwGuiPlugin*, int32_t, int32_t) { ++g_canvasCreated; return reinterpret_cast<FwGuiCanvas*>(0x5); }
void canvasDestroy(FwGuiHost*, FwGuiCanvas*) { ++g_canvasDestroyed; }
struct Box {
    explicit Box(const fwgui::Control& c) : control(c) {}
    ~Box() { ++g_destroyed; }
    void Paint(fwgui::Canvas&, const FwGuiPaintInfo& info) { painted_w = info.width; value = control.Number("value"); }
    bool Input(const FwGuiInput& in) { control.Set("value", "9"); return in.kind == FWGUI_INPUT_MOUSE_DOWN; }
    fwgui::Control control; double painted_w = 0, value = 0;
};
}

int main() {
    FwGuiPanelControlApi panel{}; panel.struct_size = sizeof panel;
    panel.register_type = reg; panel.get_number = num; panel.set_property = set;
    FwGuiCanvasApi canvasApi{}; canvasApi.struct_size = sizeof canvasApi;
    canvasApi.create = canvasCreate; canvasApi.destroy = canvasDestroy;
    FwGuiHostApi api{}; api.struct_size = sizeof api; api.panel_control = &panel; api.canvas = &canvasApi;
    fwgui::Host host(reinterpret_cast<FwGuiHost*>(0x1), &api, reinterpret_cast<FwGuiPlugin*>(0x2));

    fwgui::ControlTypeInfo info; info.id = "box"; info.label = "Box"; info.width = 50; info.height = 20;
    assert(fwgui::RegisterControlType<Box>(host, info) == FWGUI_OK);
    assert(std::strcmp(g_registered.id, "box") == 0);
    assert(g_registered.default_width == 50 && g_registered.callbacks.paint && g_registered.callbacks.input);
    assert(g_registered.callbacks.property_changed == nullptr); // Box has no PropertyChanged

    void* state = g_registered.callbacks.create(g_registered.type_user, reinterpret_cast<FwGuiControl*>(0x3));
    assert(state);
    FwGuiPaintInfo pi{}; pi.struct_size = sizeof pi; pi.width = 42;
    g_registered.callbacks.paint(state, reinterpret_cast<FwGuiCanvas*>(0x4), &pi);
    assert(static_cast<Box*>(state)->painted_w == 42 && static_cast<Box*>(state)->value == 7.5);
    FwGuiInput in{}; in.struct_size = sizeof in; in.kind = FWGUI_INPUT_MOUSE_DOWN;
    assert(g_registered.callbacks.input(state, &in) == 1 && g_lastSet == "value=9");
    g_registered.callbacks.destroy(state);
    assert(g_destroyed == 1);

    // A borrowed canvas never destroys its handle, even when moved.
    {
        fwgui::Canvas borrowed = fwgui::Canvas::Borrow(host, reinterpret_cast<FwGuiCanvas*>(0x4));
        assert(borrowed);
    }
    assert(g_canvasDestroyed == 0);
    {
        fwgui::Canvas b1 = fwgui::Canvas::Borrow(host, reinterpret_cast<FwGuiCanvas*>(0x4));
        fwgui::Canvas moved(std::move(b1));
        fwgui::Canvas assigned;
        assigned = std::move(moved);
        assert(assigned && !moved && !b1);
    }
    assert(g_canvasDestroyed == 0);

    // An owned canvas destroys exactly once.
    {
        fwgui::Canvas owned(host, 10, 10);
        assert(owned && g_canvasCreated == 1);
    }
    assert(g_canvasDestroyed == 1);
    return 0;
}
