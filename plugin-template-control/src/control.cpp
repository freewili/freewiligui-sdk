#include <freewiligui/panel_control.hpp>
#include <cstdio>

namespace {
const FwGuiPropertyDesc kProps[] = {
    {sizeof(FwGuiPropertyDesc), "value", FWGUI_PROP_NUMBER, "0", 0, 0, nullptr, "Number to display"},
};

struct MyControl {
    explicit MyControl(const fwgui::Control& c) : control(c) {}
    fwgui::Control control;   // required public member
    void Paint(fwgui::Canvas& c, const FwGuiPaintInfo& info) {
        c.SetFillStyle(0xFF3A3F4Bu);
        c.FillRect(0, 0, info.width, info.height);
        char text[32]; std::snprintf(text, sizeof text, "%g", control.Number("value"));
        c.SetFillStyle(0xFFFFFFFFu);
        c.SetFont(14);
        c.FillText(text, 8, info.height / 2);
    }
};

class MyControlPlugin : public fwgui::ControlPlugin<MyControlPlugin> {
public:
    FwGuiResult OnStartup(fwgui::Host host) {
        fwgui::ControlTypeInfo info;
        info.id = "my-control"; info.label = "My Control";
        info.width = 120; info.height = 40;
        info.properties = kProps; info.property_count = 1;
        return fwgui::RegisterControlType<MyControl>(host, info);
    }
};

const FwGuiPluginDesc kDesc = {
    sizeof(FwGuiPluginDesc), "com.example.my-control", "My Control", "0.1.0",
    "", "A minimal panel control.", nullptr, FWGUI_FINGERPRINT_INIT_NO_IMGUI, FWGUI_PLUGIN_NO_IMGUI
};
}
FWGUI_PLUGIN_MAIN_NO_IMGUI(my_control, &kDesc, MyControlPlugin)
