#include <freewiligui/panel_control.hpp>
#include <cassert>
#include <cstring>
#include <stdexcept>
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
// A constructor that throws: create returns NULL (the host binds a blank
// control) and the reason reaches the host log as an error.
struct Throws {
    explicit Throws(const fwgui::Control& c) : control(c) { throw std::runtime_error("no sensor"); }
    void Paint(fwgui::Canvas&, const FwGuiPaintInfo&) {}
    fwgui::Control control;
};
// Callbacks that throw: each is contained, and the first one per control type
// reaches the host log as an error.
struct ThrowsLater {
    explicit ThrowsLater(const fwgui::Control& c) : control(c) {}
    void Paint(fwgui::Canvas&, const FwGuiPaintInfo&) { throw std::runtime_error("paint broke"); }
    bool Input(const FwGuiInput&) { throw std::runtime_error("input broke"); }
    void PropertyChanged(std::string_view) { throw 42; }
    fwgui::Control control;
};
std::string g_log; FwGuiLogLevel g_logLevel = FWGUI_LOG_DEBUG; int g_logs = 0;
void logFn(FwGuiHost*, FwGuiPlugin*, FwGuiLogLevel level, const char* message) { g_logLevel = level; g_log = message ? message : ""; ++g_logs; }
}

int main() {
    FwGuiPanelControlApi panel{}; panel.struct_size = sizeof panel;
    panel.register_type = reg; panel.get_number = num; panel.set_property = set;
    FwGuiCanvasApi canvasApi{}; canvasApi.struct_size = sizeof canvasApi;
    canvasApi.create = canvasCreate; canvasApi.destroy = canvasDestroy;
    FwGuiCoreApi core{}; core.struct_size = sizeof core; core.log = logFn;
    FwGuiHostApi api{}; api.struct_size = sizeof api; api.panel_control = &panel; api.canvas = &canvasApi; api.core = &core;
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

    // A throwing constructor: NULL state, and an error log that names it.
    fwgui::ControlTypeInfo tinfo; tinfo.id = "throws"; tinfo.label = "Throws";
    assert(fwgui::RegisterControlType<Throws>(host, tinfo) == FWGUI_OK);
    void* none = g_registered.callbacks.create(g_registered.type_user, reinterpret_cast<FwGuiControl*>(0x3));
    assert(none == nullptr);
    assert(g_logLevel == FWGUI_LOG_ERROR);
    assert(g_log.find("create") != std::string::npos && g_log.find("no sensor") != std::string::npos);

    // Paint, input and property_changed exceptions are contained and logged
    // once per control type, not once per call.
    fwgui::ControlTypeInfo linfo; linfo.id = "later"; linfo.label = "Later";
    assert(fwgui::RegisterControlType<ThrowsLater>(host, linfo) == FWGUI_OK);
    const FwGuiControlTypeDesc later = g_registered;
    void* ls = later.callbacks.create(later.type_user, reinterpret_cast<FwGuiControl*>(0x3));
    assert(ls);
    g_logs = 0; g_log.clear(); g_logLevel = FWGUI_LOG_DEBUG;
    later.callbacks.paint(ls, reinterpret_cast<FwGuiCanvas*>(0x4), &pi);
    assert(g_logs == 1 && g_logLevel == FWGUI_LOG_ERROR);
    assert(g_log.find("paint") != std::string::npos && g_log.find("paint broke") != std::string::npos);
    later.callbacks.paint(ls, reinterpret_cast<FwGuiCanvas*>(0x4), &pi);
    assert(later.callbacks.input(ls, &in) == 0);
    later.callbacks.property_changed(ls, "value");
    assert(g_logs == 1);
    later.callbacks.destroy(ls);
    return 0;
}
