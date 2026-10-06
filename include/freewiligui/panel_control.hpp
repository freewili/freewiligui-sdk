#ifndef FREEWILIGUI_PANEL_CONTROL_HPP
#define FREEWILIGUI_PANEL_CONTROL_HPP
// ImGui-free C++ helpers for panel controls. Never include ImGui from here.
#include <freewiligui/host.hpp>
#include <string>

namespace fwgui {
template<class Derived> using ControlPlugin = PluginBase<Derived, detail::NoUi>;

class Control {
public:
    Control() = default;
    Control(const Host& host, FwGuiControl* control) : host_(host), control_(control) {}
    FwGuiControl* Raw() const noexcept { return control_; }
    const Host& GetHost() const noexcept { return host_; }
    double Number(const char* name) const { auto* t = Table(); return t && t->get_number ? t->get_number(host_.RawHost(), control_, name) : 0.0; }
    std::string_view String(const char* name) const { auto* t = Table(); return t && t->get_string ? StringView(t->get_string(host_.RawHost(), control_, name)) : std::string_view(); }
    FwGuiResult Set(const char* name, const char* value) const { auto* t = Table(); return t && t->set_property ? t->set_property(host_.RawHost(), control_, name, value) : FWGUI_ERR_UNSUPPORTED; }
    bool ReadNumber(const char* prop, double& out) const {
        auto* t = Table(); if (!t || !t->read_signal) return false;
        FwGuiStrView text{}; int32_t isText = 0;
        return t->read_signal(host_.RawHost(), control_, prop, &out, &text, &isText) == FWGUI_OK && !isText;
    }
    bool ReadText(const char* prop, std::string& out) const {
        auto* t = Table(); if (!t || !t->read_signal) return false;
        double n = 0; FwGuiStrView text{}; int32_t isText = 0;
        if (t->read_signal(host_.RawHost(), control_, prop, &n, &text, &isText) != FWGUI_OK) return false;
        out = isText ? std::string(StringView(text)) : std::to_string(n); return true;
    }
    FwGuiResult Write(const char* prop, double v) const { auto* t = Table(); return t && t->write_signal ? t->write_signal(host_.RawHost(), control_, prop, v, nullptr) : FWGUI_ERR_UNSUPPORTED; }
    FwGuiResult WriteText(const char* prop, const char* v) const { auto* t = Table(); return t && t->write_signal ? t->write_signal(host_.RawHost(), control_, prop, 0.0, v) : FWGUI_ERR_UNSUPPORTED; }
    FwGuiResult RequestFocus() const { auto* t = Table(); return t && t->request_focus ? t->request_focus(host_.RawHost(), control_) : FWGUI_ERR_UNSUPPORTED; }
    FwGuiResult ReleaseFocus() const { auto* t = Table(); return t && t->release_focus ? t->release_focus(host_.RawHost(), control_) : FWGUI_ERR_UNSUPPORTED; }
private:
    const FwGuiPanelControlApi* Table() const {
        const auto* a = host_.Api();
        return a && FWGUI_HAS(a, panel_control) ? a->panel_control : nullptr;
    }
    Host host_; FwGuiControl* control_ = nullptr;
};

struct ControlTypeInfo {
    const char* id = nullptr; const char* label = nullptr; const char* section = nullptr;
    double width = 120, height = 90;
    const FwGuiPropertyDesc* properties = nullptr; uint32_t property_count = 0;
    const void* icon_png = nullptr; uint32_t icon_png_size = 0;
};

namespace detail {
template<class T,class=void> struct HasInput : std::false_type {};
template<class T> struct HasInput<T, std::void_t<decltype(std::declval<T&>().Input(std::declval<const FwGuiInput&>()))>> : std::true_type {};
template<class T,class=void> struct HasPropertyChanged : std::false_type {};
template<class T> struct HasPropertyChanged<T, std::void_t<decltype(std::declval<T&>().PropertyChanged(std::string_view()))>> : std::true_type {};
template<class T> struct ControlThunks {
    // type_user is a heap Host copy owned for the type's lifetime.
    static void* Create(void* user, FwGuiControl* c) noexcept {
        try { return new T(Control(*static_cast<Host*>(user), c)); } catch (...) { return nullptr; }
    }
    static void Destroy(void* s) noexcept { delete static_cast<T*>(s); }
    static void Paint(void* s, FwGuiCanvas* c, const FwGuiPaintInfo* i) noexcept {
        if (!s || !i) return; auto* self = static_cast<T*>(s);
        try { Canvas canvas = Canvas::Borrow(self->control.GetHost(), c); self->Paint(canvas, *i); } catch (...) {}
    }
    static int Input(void* s, const FwGuiInput* in) noexcept {
        if (!s || !in) return 0;
        try { return static_cast<T*>(s)->Input(*in) ? 1 : 0; } catch (...) { return 0; }
    }
    static void PropertyChanged(void* s, const char* n) noexcept {
        if (!s || !n) return;
        try { static_cast<T*>(s)->PropertyChanged(std::string_view(n)); } catch (...) {}
    }
};
}

// T: T(const Control&), a public `Control control;` member, and
// void Paint(Canvas&, const FwGuiPaintInfo&). Optional: bool Input(const FwGuiInput&),
// void PropertyChanged(std::string_view). Call from OnStartup.
template<class T>
FwGuiResult RegisterControlType(const Host& host, const ControlTypeInfo& info) {
    const auto* a = host.Api();
    const auto* t = a && FWGUI_HAS(a, panel_control) ? a->panel_control : nullptr;
    if (!t || !t->register_type) return FWGUI_ERR_UNSUPPORTED;
    static Host s_host; s_host = host;   // one per T; lives for the module's life
    FwGuiControlTypeDesc d{};
    d.struct_size = sizeof d; d.id = info.id; d.label = info.label; d.palette_section = info.section;
    d.icon_png = info.icon_png; d.icon_png_size = info.icon_png_size;
    d.default_width = info.width; d.default_height = info.height;
    d.properties = info.properties; d.property_count = info.property_count;
    d.callbacks.struct_size = sizeof d.callbacks;
    d.callbacks.create = &detail::ControlThunks<T>::Create;
    d.callbacks.destroy = &detail::ControlThunks<T>::Destroy;
    d.callbacks.paint = &detail::ControlThunks<T>::Paint;
    if constexpr (detail::HasInput<T>::value) d.callbacks.input = &detail::ControlThunks<T>::Input;
    if constexpr (detail::HasPropertyChanged<T>::value) d.callbacks.property_changed = &detail::ControlThunks<T>::PropertyChanged;
    d.type_user = &s_host;
    return t->register_type(host.RawHost(), host.Plugin(), &d);
}
}

#define FWGUI_PLUGIN_MAIN_NO_IMGUI(symbol_prefix, desc_ptr, plugin_type) \
    FWGUI_PLUGIN_DEFINE_EX(symbol_prefix, desc_ptr, plugin_type::VTable(), plugin_type::Create)
#endif