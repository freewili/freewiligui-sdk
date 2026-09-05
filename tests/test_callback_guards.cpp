#include <freewiligui/plugin.hpp>

#include <cassert>
#include <stdexcept>
#include <string_view>

namespace {
FwGuiViewDesc captured_view{};
FwGuiMcpToolFn captured_tool = nullptr;
int log_count = 0;
int ok_replies = 0;
int text_replies = 0;
int error_replies = 0;

void log_message(FwGuiHost*, FwGuiPlugin*, FwGuiLogLevel, const char*) {
    ++log_count;
}
FwGuiResult register_view(FwGuiHost*, FwGuiPlugin*, const FwGuiViewDesc* desc) {
    captured_view = *desc;
    return FWGUI_OK;
}
FwGuiResult register_tool(FwGuiHost*, FwGuiPlugin*, const char*, const char*,
                          const char*, FwGuiMcpToolFn handler, void*) {
    captured_tool = handler;
    return FWGUI_OK;
}
void reply_ok(FwGuiMcpReply*, const char*) { ++ok_replies; }
void reply_text(FwGuiMcpReply*, const char*) { ++text_replies; }
void reply_error(FwGuiMcpReply*, const char*) { ++error_replies; }

class ThrowingCallbacks : public fwgui::Plugin<ThrowingCallbacks> {
public:
    FwGuiResult OnStartup(fwgui::Host) {
        fwgui::View view;
        view.id = "throwing";
        view.title = "Throwing";
        if (RegisterView<&ThrowingCallbacks::Draw>(view) != FWGUI_OK)
            return FWGUI_ERR_FAILED;
        return RegisterTool<&ThrowingCallbacks::Tool>("throwing", "", "{}");
    }
    void Draw(const fwgui::ViewCtx&) { throw std::runtime_error("draw"); }
    void OnFrame(const FwGuiFrameInfo&) { throw std::runtime_error("frame"); }
    void OnEvent(FwGuiEvent, const FwGuiEventData*) {
        throw std::runtime_error("event");
    }
    void Tool(std::string_view args, const fwgui::McpReply& reply) {
        if (args == "after") {
            reply.Text("first");
            throw std::runtime_error("after reply");
        }
        if (args == "twice") {
            reply.Text("first");
            reply.Error("second");
            return;
        }
        throw std::runtime_error("before reply");
    }
};

template<class F>
bool escaped(F&& function) {
    try {
        function();
        return false;
    } catch (...) {
        return true;
    }
}
}

int main() {
    FwGuiCoreApi core{};
    core.struct_size = sizeof(core);
    core.log = &log_message;
    FwGuiUiApi ui{};
    ui.struct_size = sizeof(ui);
    ui.register_view = &register_view;
    FwGuiMcpApi mcp{};
    mcp.struct_size = sizeof(mcp);
    mcp.register_tool = &register_tool;
    mcp.reply_ok = &reply_ok;
    mcp.reply_text = &reply_text;
    mcp.reply_error = &reply_error;
    FwGuiHostApi api{};
    api.struct_size = sizeof(api);
    api.abi_version = FWGUI_ABI_VERSION;
    api.core = &core;
    api.ui = &ui;
    api.mcp = &mcp;

    const FwGuiPluginVTable* table = ThrowingCallbacks::VTable();
    FwGuiPlugin* plugin = ThrowingCallbacks::Create();
    assert(table->startup(plugin, reinterpret_cast<FwGuiHost*>(1), &api) == FWGUI_OK);
    assert(captured_view.draw != nullptr);
    assert(captured_tool != nullptr);

    FwGuiFrameInfo frame{};
    frame.struct_size = sizeof(frame);
    assert(!escaped([&] { table->frame(plugin, &frame); }));
    FwGuiEventData event_data{};
    event_data.struct_size = sizeof(event_data);
    assert(!escaped([&] {
        table->event(plugin, FWGUI_EVENT_THEME_CHANGED, &event_data);
    }));
    FwGuiViewCtx view_ctx{};
    view_ctx.struct_size = sizeof(view_ctx);
    view_ctx.host = reinterpret_cast<FwGuiHost*>(1);
    assert(!escaped([&] { captured_view.draw(plugin, &view_ctx); }));
    assert(log_count == 3);

    FwGuiMcpReply* reply = reinterpret_cast<FwGuiMcpReply*>(2);
    assert(!escaped([&] { captured_tool(plugin, "before", reply, nullptr); }));
    assert(error_replies == 1);
    assert(ok_replies == 0);
    assert(text_replies == 0);

    ok_replies = text_replies = error_replies = 0;
    assert(!escaped([&] { captured_tool(plugin, "after", reply, nullptr); }));
    assert(text_replies == 1);
    assert(error_replies == 0);

    ok_replies = text_replies = error_replies = 0;
    captured_tool(plugin, "twice", reply, nullptr);
    assert(text_replies == 1);
    assert(error_replies == 0);

    table->shutdown(plugin);
    return 0;
}
