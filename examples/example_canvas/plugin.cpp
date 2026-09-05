#include <freewiligui/plugin.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>

class CanvasExample : public fwgui::Plugin<CanvasExample> {
public:
    FwGuiResult OnStartup(fwgui::Host) {
        RegisterDoc("index", "Canvas Gauge",
            "# Canvas Gauge\nA moving gauge drawn with the optional Canvas service.");
        fwgui::View view;
        view.id = "canvas";
        view.title = "Canvas Gauge";
        view.menu_group = "Examples";
        return RegisterView<&CanvasExample::Draw>(view);
    }
    void Draw(const fwgui::ViewCtx& ctx) {
        const auto& host = ctx.host();
        if (!host.HasCanvas()) {
            ImGui::TextDisabled("This host does not provide Canvas.");
            return;
        }
        ImVec2 available = ImGui::GetContentRegionAvail();
        int32_t width = std::max(128, static_cast<int32_t>(available.x));
        int32_t height = std::max(96, static_cast<int32_t>(available.y));
        if (!canvas_) canvas_ = fwgui::Canvas(host, width, height);
        if (!canvas_) return;
        if (canvas_.Width() != width || canvas_.Height() != height)
            canvas_.Resize(width, height);
        double value = 0.5 + 0.5 * std::sin(host.HostTime());
        double cx = width * 0.5;
        double cy = height * 0.58;
        double radius = std::min(width, height) * 0.30;
        double start = 0.75 * 3.141592653589793;
        double finish = 2.25 * 3.141592653589793;
        canvas_.Begin(0xff18232du);
        canvas_.SetLineWidth(12);
        canvas_.SetStrokeStyle(0xff405060u);
        canvas_.BeginPath();
        canvas_.Arc(cx, cy, radius, start, finish);
        canvas_.Stroke();
        canvas_.SetStrokeStyle(0xff50be98u);
        canvas_.BeginPath();
        canvas_.Arc(cx, cy, radius, start, start + (finish - start) * value);
        canvas_.Stroke();
        char label[32];
        std::snprintf(label, sizeof(label), "%.0f %%", value * 100.0);
        canvas_.SetFont(22, true);
        canvas_.SetTextAlign(FWGUI_CANVAS_ALIGN_CENTER);
        canvas_.SetFillStyle(0xffffffffu);
        canvas_.FillText(label, cx, cy);
        canvas_.End();
        canvas_.DrawImGui(ImVec2(static_cast<float>(width), static_cast<float>(height)));
    }
private:
    fwgui::Canvas canvas_;
};

static const FwGuiPluginDesc descriptor = {
    sizeof(FwGuiPluginDesc), "org.freewili.canvas", "Canvas Gauge", "1.0.0",
    "FreeWili", "Shows portable Canvas drawing from a plugin.", nullptr,
    FWGUI_FINGERPRINT_INIT
};
FWGUI_PLUGIN_MAIN(freewili_example_canvas, &descriptor, CanvasExample)
