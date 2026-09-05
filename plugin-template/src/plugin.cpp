#include <freewiligui/plugin.hpp>

class MyPlugin : public fwgui::Plugin<MyPlugin> {
public:
    FwGuiResult OnStartup(fwgui::Host) {
        RegisterDoc("index", "My Plugin", "# My Plugin\nA minimal native plugin.");
        fwgui::View view;
        view.id = "main";
        view.title = "My Plugin";
        return RegisterView<&MyPlugin::Draw>(view);
    }
    void Draw(const fwgui::ViewCtx&) { ImGui::TextUnformatted("Hello from my plugin."); }
};

static const FwGuiPluginDesc descriptor = {
    sizeof(FwGuiPluginDesc), "org.example.my-plugin", "My Plugin", "0.1.0",
    "", "A minimal FreeWili GUI plugin.", nullptr, FWGUI_FINGERPRINT_INIT
};
FWGUI_PLUGIN_MAIN(my_plugin, &descriptor, MyPlugin)
