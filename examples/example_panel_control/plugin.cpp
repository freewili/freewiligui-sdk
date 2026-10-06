#include <freewiligui/panel_control.hpp>
#include <algorithm>
#include <cstdio>

namespace {
const FwGuiPropertyDesc kProps[] = {
    {sizeof(FwGuiPropertyDesc), "value", FWGUI_PROP_NUMBER, "50", 0, 100, nullptr, "Current position (0-100)"},
    {sizeof(FwGuiPropertyDesc), "binding", FWGUI_PROP_SIGNAL_WRITE, "", 0, 0, nullptr, "Signal written when the slider moves"},
    {sizeof(FwGuiPropertyDesc), "source", FWGUI_PROP_SIGNAL_READ, "", 0, 0, nullptr, "Optional signal the slider follows"},
    {sizeof(FwGuiPropertyDesc), "track_color", FWGUI_PROP_COLOR, "#3a3f4b", 0, 0, nullptr, "Track color"},
    {sizeof(FwGuiPropertyDesc), "knob_color", FWGUI_PROP_COLOR, "#4da3ff", 0, 0, nullptr, "Knob color"},
};

struct Slider {
    explicit Slider(const fwgui::Control& c) : control(c) {}
    fwgui::Control control;
    bool dragging = false;
    double width = 1;

    double Value() const {
        double v = control.Number("value");
        double followed = 0;
        if (!dragging && control.ReadNumber("source", followed)) v = followed;
        return std::clamp(v, 0.0, 100.0);
    }
    void SetValue(double v) {
        v = std::clamp(v, 0.0, 100.0);
        char text[32]; std::snprintf(text, sizeof text, "%.3g", v);
        control.Set("value", text);
        control.Write("binding", v);
    }
    void Paint(fwgui::Canvas& c, const FwGuiPaintInfo& info) {
        width = info.width;
        const double cy = info.height / 2, pad = 10, trackW = std::max(1.0, info.width - 2 * pad);
        const double v = Value(), knobX = pad + trackW * v / 100.0;
        c.SetFillStyle(static_cast<uint32_t>(control.Number("track_color")));
        c.FillRect(pad, cy - 3, trackW, 6);
        c.SetFillStyle(static_cast<uint32_t>(control.Number("knob_color")));
        c.FillRect(pad, cy - 3, knobX - pad, 6);
        c.SetStrokeStyle(0xFFFFFFFFu);
        c.FillStrokeRoundRect(knobX - 7, cy - 11, 14, 22, 4);
        if (info.flags & FWGUI_PAINT_FOCUSED) { c.SetStrokeStyle(0xFF4DA3FFu); c.StrokeRect(1, 1, info.width - 2, info.height - 2); }
        char text[16]; std::snprintf(text, sizeof text, "%.0f", v);
        c.SetFillStyle(0xFFE0E0E0u); c.SetFont(11); c.FillText(text, pad, cy - 14);
    }
    double ValueAt(double x) const { return (x - 10) / std::max(1.0, width - 20) * 100.0; }
    bool Input(const FwGuiInput& in) {
        switch (in.kind) {
        case FWGUI_INPUT_MOUSE_DOWN: dragging = true; control.RequestFocus(); SetValue(ValueAt(in.x)); return true;
        case FWGUI_INPUT_MOUSE_MOVE: if (dragging) { SetValue(ValueAt(in.x)); return true; } return false;
        case FWGUI_INPUT_MOUSE_UP: dragging = false; return true;
        case FWGUI_INPUT_WHEEL: SetValue(Value() + (in.wheel > 0 ? 1 : -1)); return true;
        case FWGUI_INPUT_KEY_DOWN:
            if (in.key == FWGUI_KEY_LEFT || in.key == FWGUI_KEY_DOWN) { SetValue(Value() - 1); return true; }
            if (in.key == FWGUI_KEY_RIGHT || in.key == FWGUI_KEY_UP) { SetValue(Value() + 1); return true; }
            return false;
        default: return false;
        }
    }
};

class SliderPlugin : public fwgui::ControlPlugin<SliderPlugin> {
public:
    FwGuiResult OnStartup(fwgui::Host host) {
        fwgui::ControlTypeInfo info;
        info.id = "slider"; info.label = "Slider"; info.section = "Instruments";
        info.width = 200; info.height = 44;
        info.properties = kProps; info.property_count = sizeof kProps / sizeof kProps[0];
        return fwgui::RegisterControlType<Slider>(host, info);
    }
};

const FwGuiPluginDesc kDesc = {
    sizeof(FwGuiPluginDesc), "org.freewili.panel-slider", "Panel Slider", "1.0.0", "FreeWili",
    "A slider control for Graphical Panels", nullptr, FWGUI_FINGERPRINT_INIT_NO_IMGUI, FWGUI_PLUGIN_NO_IMGUI
};
}
FWGUI_PLUGIN_MAIN_NO_IMGUI(freewili_panel_slider, &kDesc, SliderPlugin)
