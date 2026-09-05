#include <freewiligui/plugin.hpp>

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include "gui_stubs.h"

namespace {
double late_time(FwGuiHost*) { return 42.0; }
FwGuiStrView late_commands(FwGuiHost*) { return {"unsafe", 6}; }
int canvas_created = 0;
int canvas_destroyed = 0;
int texture_reads = 0;
FwGuiCanvas* create_canvas(FwGuiHost*, FwGuiPlugin*, int32_t, int32_t) {
    ++canvas_created;
    return reinterpret_cast<FwGuiCanvas*>(3);
}
int32_t canvas_width(FwGuiHost*, FwGuiCanvas*) { return 32; }
int32_t canvas_height(FwGuiHost*, FwGuiCanvas*) { return 24; }
void destroy_canvas(FwGuiHost*, FwGuiCanvas*) { ++canvas_destroyed; }
uint64_t canvas_texture(FwGuiHost*, FwGuiCanvas*) {
    ++texture_reads;
    return 7;
}
int destroyed = 0;
struct FailedPlugin : fwgui::Plugin<FailedPlugin> {
    ~FailedPlugin() { ++destroyed; }
    FwGuiResult OnStartup(fwgui::Host) { return FWGUI_ERR_FAILED; }
};
int shutdown_called = 0;
struct ShutdownPlugin : fwgui::Plugin<ShutdownPlugin> {
    FwGuiResult OnStartup(fwgui::Host) { return FWGUI_OK; }
    void OnShutdown() { ++shutdown_called; }
};
int throw_startup_destroyed = 0;
struct ThrowStartupPlugin : fwgui::Plugin<ThrowStartupPlugin> {
    ~ThrowStartupPlugin() { ++throw_startup_destroyed; }
    FwGuiResult OnStartup(fwgui::Host) { throw std::runtime_error("startup"); }
};
int throw_shutdown_destroyed = 0;
struct ThrowShutdownPlugin : fwgui::Plugin<ThrowShutdownPlugin> {
    ~ThrowShutdownPlugin() { ++throw_shutdown_destroyed; }
    FwGuiResult OnStartup(fwgui::Host) { return FWGUI_OK; }
    void OnShutdown() { throw std::runtime_error("shutdown"); }
};
}

int main() {
    fwgui::Host empty;
    assert(!empty.HasCore());
    assert(!empty.HasCanvas());
    assert(empty.DpiScale() == 1.0f);
    assert(empty.HostTime() == 0.0);

    FwGuiCoreApi short_core{};
    short_core.struct_size = static_cast<uint32_t>(offsetof(FwGuiCoreApi, host_name));
    short_core.host_time = &late_time;
    FwGuiHostApi root{};
    root.struct_size = sizeof(root);
    root.abi_version = FWGUI_ABI_VERSION;
    root.core = &short_core;
    fwgui::Host short_host(reinterpret_cast<FwGuiHost*>(1), &root,
                           reinterpret_cast<FwGuiPlugin*>(2));
    assert(short_host.HostTime() == 0.0);

    FwGuiHostApi short_root{};
    short_root.struct_size = static_cast<uint32_t>(offsetof(FwGuiHostApi, canvas));
    short_root.canvas = reinterpret_cast<const FwGuiCanvasApi*>(1);
    fwgui::Host guarded(reinterpret_cast<FwGuiHost*>(1), &short_root);
    assert(!guarded.HasCanvas());

    FwGuiFreeWiliApi short_device{};
    short_device.struct_size =
        static_cast<uint32_t>(offsetof(FwGuiFreeWiliApi, list_commands));
    short_device.list_commands = &late_commands;
    root.freewili = &short_device;
    assert(short_host.FreeWiliListCommands().empty());

    FwGuiCanvasApi canvas_api{};
    canvas_api.struct_size = sizeof(canvas_api);
    canvas_api.create = &create_canvas;
    canvas_api.destroy = &destroy_canvas;
    canvas_api.width = &canvas_width;
    canvas_api.height = &canvas_height;
    canvas_api.texture_id = &canvas_texture;
    root.canvas = &canvas_api;
    {
        fwgui::Canvas canvas(short_host, 32, 24);
        assert(canvas);
        assert(canvas_created == 1);
        canvas.DrawImGui();
        assert(texture_reads == 1);
        assert(freewiligui_test_image_width == 32);
        assert(freewiligui_test_image_height == 24);
    }
    assert(canvas_destroyed == 1);

    const FwGuiPluginVTable* table = FailedPlugin::VTable();
    FwGuiPlugin* instance = FailedPlugin::Create();
    assert(instance != nullptr);
    assert(table->startup(instance, reinterpret_cast<FwGuiHost*>(1), &root)
           == FWGUI_ERR_FAILED);
    assert(destroyed == 1);
    assert(table->frame == nullptr);
    assert(table->event == nullptr);

    const FwGuiPluginVTable* shutdown_table = ShutdownPlugin::VTable();
    FwGuiPlugin* shutdown_instance = ShutdownPlugin::Create();
    assert(shutdown_table->startup(shutdown_instance,
                                   reinterpret_cast<FwGuiHost*>(1), &root)
           == FWGUI_OK);
    shutdown_table->shutdown(shutdown_instance);
    assert(shutdown_called == 1);

    bool startup_escaped = false;
    FwGuiResult startup_result = FWGUI_OK;
    try {
        FwGuiPlugin* value = ThrowStartupPlugin::Create();
        startup_result = ThrowStartupPlugin::VTable()->startup(
            value, reinterpret_cast<FwGuiHost*>(1), &root);
    } catch (...) {
        startup_escaped = true;
    }
    assert(!startup_escaped);
    assert(startup_result == FWGUI_ERR_FAILED);
    assert(throw_startup_destroyed == 1);

    bool shutdown_escaped = false;
    FwGuiPlugin* value = ThrowShutdownPlugin::Create();
    assert(ThrowShutdownPlugin::VTable()->startup(
               value, reinterpret_cast<FwGuiHost*>(1), &root) == FWGUI_OK);
    try {
        ThrowShutdownPlugin::VTable()->shutdown(value);
    } catch (...) {
        shutdown_escaped = true;
    }
    assert(!shutdown_escaped);
    assert(throw_shutdown_destroyed == 1);
    return 0;
}
