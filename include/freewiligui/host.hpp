#ifndef FREEWILIGUI_HOST_HPP
#define FREEWILIGUI_HOST_HPP

#include <freewiligui/host_api.h>

#include <cstddef>
#include <cstdint>
#include <new>
#include <string_view>
#include <type_traits>
#include <utility>

namespace fwgui {

inline std::string_view StringView(FwGuiStrView value) noexcept {
    return value.data && value.len > 0
        ? std::string_view(value.data, static_cast<std::size_t>(value.len))
        : std::string_view();
}

class McpReply {
public:
    McpReply() = default;
    McpReply(const FwGuiMcpApi* api, FwGuiMcpReply* reply) : api_(api), reply_(reply) {}
    McpReply(const McpReply&) = delete;
    McpReply& operator=(const McpReply&) = delete;
    void Ok(const char* json) const {
        if (!replied_ && FWGUI_HAS(api_, reply_ok) && api_->reply_ok) {
            replied_ = true;
            api_->reply_ok(reply_, json ? json : "{}");
        }
    }
    void Text(const char* value) const {
        if (!replied_ && FWGUI_HAS(api_, reply_text) && api_->reply_text) {
            replied_ = true;
            api_->reply_text(reply_, value ? value : "");
        }
    }
    void Error(const char* value) const {
        if (!replied_ && FWGUI_HAS(api_, reply_error) && api_->reply_error) {
            replied_ = true;
            api_->reply_error(reply_, value ? value : "");
        }
    }
    bool HasReplied() const noexcept { return replied_; }
private:
    const FwGuiMcpApi* api_ = nullptr;
    FwGuiMcpReply* reply_ = nullptr;
    mutable bool replied_ = false;
};

class Host {
public:
    Host() = default;
    Host(FwGuiHost* host, const FwGuiHostApi* api, FwGuiPlugin* plugin = nullptr)
        : host_(host), api_(api), plugin_(plugin) {}

    FwGuiHost* RawHost() const noexcept { return host_; }
    const FwGuiHostApi* Api() const noexcept { return api_; }
    FwGuiPlugin* Plugin() const noexcept { return plugin_; }

    bool HasCore() const noexcept { return api_ && FWGUI_HAS(api_, core) && api_->core; }
    bool HasUi() const noexcept { return api_ && FWGUI_HAS(api_, ui) && api_->ui; }
    bool HasCanvas() const noexcept { return api_ && FWGUI_HAS(api_, canvas) && api_->canvas; }
    bool HasMcp() const noexcept { return api_ && FWGUI_HAS(api_, mcp) && api_->mcp; }
    bool HasFreeWili() const noexcept { return api_ && FWGUI_HAS(api_, freewili) && api_->freewili; }

    uint32_t AbiVersion() const { const auto*t=Core();return t&&FWGUI_HAS(t,abi_version)&&t->abi_version?t->abi_version(host_):0; }
    std::string_view HostVersion() const { const auto*t=Core();return t&&FWGUI_HAS(t,host_version)&&t->host_version?StringView(t->host_version(host_)):std::string_view(); }
    std::string_view HostName() const { const auto*t=Core();return t&&FWGUI_HAS(t,host_name)&&t->host_name?StringView(t->host_name(host_)):std::string_view(); }
    std::string_view LastError() const { const auto*t=Core();return t&&FWGUI_HAS(t,last_error)&&t->last_error?StringView(t->last_error(host_)):std::string_view(); }
    bool IsMainThread() const { const auto*t=Core();return t&&FWGUI_HAS(t,is_main_thread)&&t->is_main_thread&&t->is_main_thread(host_)!=0; }
    void AssertMainThread(const char* where) const {
        const auto* t = Core();
        if (t && FWGUI_HAS(t, assert_main_thread) && t->assert_main_thread) t->assert_main_thread(host_, where);
    }
    void Log(FwGuiLogLevel level, const char* message) const {
        const auto* t = Core();
        if (t && FWGUI_HAS(t, log) && t->log) t->log(host_, plugin_, level, message ? message : "");
    }
    void LogDebug(const char* message) const { Log(FWGUI_LOG_DEBUG, message); }
    void LogInfo(const char* message) const { Log(FWGUI_LOG_INFO, message); }
    void LogWarn(const char* message) const { Log(FWGUI_LOG_WARN, message); }
    void LogError(const char* message) const { Log(FWGUI_LOG_ERROR, message); }
    std::string_view SettingsGet(const char* key) const {
        const auto* t = Core();
        return t && FWGUI_HAS(t, settings_get) && t->settings_get
            ? StringView(t->settings_get(host_, plugin_, key)) : std::string_view();
    }
    FwGuiResult SettingsSet(const char* key, const char* value) const {
        const auto* t = Core();
        return t && FWGUI_HAS(t, settings_set) && t->settings_set
            ? t->settings_set(host_, plugin_, key, value) : FWGUI_ERR_UNSUPPORTED;
    }
    double SettingsGetNumber(const char* key, double fallback) const {
        const auto* t = Core();
        return t && FWGUI_HAS(t, settings_get_number) && t->settings_get_number
            ? t->settings_get_number(host_, plugin_, key, fallback) : fallback;
    }
    FwGuiResult SettingsSetNumber(const char* key, double value) const {
        const auto* t = Core();
        return t && FWGUI_HAS(t, settings_set_number) && t->settings_set_number
            ? t->settings_set_number(host_, plugin_, key, value) : FWGUI_ERR_UNSUPPORTED;
    }
    FwGuiResult SettingsErase(const char* key) const {
        const auto* t = Core();
        return t && FWGUI_HAS(t, settings_erase) && t->settings_erase
            ? t->settings_erase(host_, plugin_, key) : FWGUI_ERR_UNSUPPORTED;
    }
    std::string_view DataDir() const { const auto*t=Core();return t&&FWGUI_HAS(t,data_dir)&&t->data_dir?StringView(t->data_dir(host_)):std::string_view(); }
    std::string_view PluginPath() const {
        const auto* t = Core();
        return t && FWGUI_HAS(t, plugin_path) && t->plugin_path
            ? StringView(t->plugin_path(host_, plugin_)) : std::string_view();
    }
    double HostTime() const { const auto*t=Core();return t&&FWGUI_HAS(t,host_time)&&t->host_time?t->host_time(host_):0.0; }

    float DpiScale() const {
        const auto* t = Ui();
        return t && FWGUI_HAS(t, dpi_scale) && t->dpi_scale ? t->dpi_scale(host_) : 1.0f;
    }
    std::string_view ThemeName() const {
        const auto* t = Ui();
        return t && FWGUI_HAS(t, theme_name) && t->theme_name
            ? StringView(t->theme_name(host_)) : std::string_view();
    }
    bool ViewIsVisible(const char* id) const {
        const auto* t = Ui();
        return t && FWGUI_HAS(t, view_is_visible) && t->view_is_visible
            && t->view_is_visible(host_, plugin_, id) != 0;
    }
    void SetViewVisible(const char* id, bool visible) const {
        const auto* t = Ui();
        if (t && FWGUI_HAS(t, set_view_visible) && t->set_view_visible)
            t->set_view_visible(host_, plugin_, id, visible ? 1 : 0);
    }
    void FocusView(const char* id) const {
        const auto* t = Ui();
        if (t && FWGUI_HAS(t, focus_view) && t->focus_view) t->focus_view(host_, plugin_, id);
    }
    void OpenDoc(const char* id) const {
        const auto* t = Ui();
        if (t && FWGUI_HAS(t, open_doc) && t->open_doc) t->open_doc(host_, plugin_, id);
    }
    const char* Tr(const char* english) const {
        const auto* t = Ui();
        return t && FWGUI_HAS(t, tr) && t->tr ? t->tr(host_, plugin_, english) : english;
    }
    void PushFont(int32_t kind) const {
        const auto* t = Ui();
        if (t && FWGUI_HAS(t, push_font) && t->push_font) t->push_font(host_, kind);
    }
    void PopFont() const {
        const auto* t = Ui();
        if (t && FWGUI_HAS(t, pop_font) && t->pop_font) t->pop_font(host_);
    }
    bool Splitter(const char* id, float* size, float extent, float min_before,
                  float min_after, bool vertical) const {
        const auto* t = Ui();
        return t && FWGUI_HAS(t, splitter) && t->splitter
            && t->splitter(host_, id, size, extent, min_before, min_after,
                           vertical ? 1 : 0) != 0;
    }

    bool McpEnabled() const {
        const auto* t = Mcp();
        return t && FWGUI_HAS(t, is_enabled) && t->is_enabled && t->is_enabled(host_) != 0;
    }
    std::string_view McpToolFullName(const char* name) const {
        const auto* t = Mcp();
        return t && FWGUI_HAS(t, tool_full_name) && t->tool_full_name
            ? StringView(t->tool_full_name(host_, plugin_, name)) : std::string_view();
    }
    FwGuiResult UnregisterTool(const char* name) const {
        const auto* t = Mcp();
        return t && FWGUI_HAS(t, unregister_tool) && t->unregister_tool
            ? t->unregister_tool(host_, plugin_, name) : FWGUI_ERR_UNSUPPORTED;
    }

    std::string_view FreeWiliListDevices() const { const auto*t=FreeWili();return t&&FWGUI_HAS(t,list_devices)&&t->list_devices?StringView(t->list_devices(host_)):std::string_view(); }
    std::string_view FreeWiliListCommands() const { const auto*t=FreeWili();return t&&FWGUI_HAS(t,list_commands)&&t->list_commands?StringView(t->list_commands(host_)):std::string_view(); }
    FwGuiResult FreeWiliSubmit(const char* device, const char* command,
                               const char* args, uint64_t* job) const {
        const auto* t = FreeWili();
        return t && FWGUI_HAS(t, submit) && t->submit
            ? t->submit(host_, plugin_, device, command, args, job) : FWGUI_ERR_UNSUPPORTED;
    }
    std::string_view FreeWiliJobResult(uint64_t job) const {
        const auto* t = FreeWili();
        return t && FWGUI_HAS(t, job_result) && t->job_result
            ? StringView(t->job_result(host_, plugin_, job)) : std::string_view();
    }
    FwGuiResult FreeWiliCancelJob(uint64_t job) const { const auto*t=FreeWili();return t&&FWGUI_HAS(t,cancel_job)&&t->cancel_job?t->cancel_job(host_,plugin_,job):FWGUI_ERR_UNSUPPORTED; }
    FwGuiResult FreeWiliReleaseJob(uint64_t job) const { const auto*t=FreeWili();return t&&FWGUI_HAS(t,release_job)&&t->release_job?t->release_job(host_,plugin_,job):FWGUI_ERR_UNSUPPORTED; }
    FwGuiResult FreeWiliSubscribeEvents(const char* device, const char* filter,
                                        uint64_t* subscription) const {
        const auto* t = FreeWili();
        return t && FWGUI_HAS(t, subscribe_events) && t->subscribe_events
            ? t->subscribe_events(host_, plugin_, device, filter, subscription)
            : FWGUI_ERR_UNSUPPORTED;
    }
    std::string_view FreeWiliReadEvents(uint64_t subscription, uint32_t max_events) const {
        const auto* t = FreeWili();
        return t && FWGUI_HAS(t, read_events) && t->read_events
            ? StringView(t->read_events(host_, plugin_, subscription, max_events))
            : std::string_view();
    }
    FwGuiResult FreeWiliUnsubscribeEvents(uint64_t subscription) const {
        const auto* t = FreeWili();
        return t && FWGUI_HAS(t, unsubscribe_events) && t->unsubscribe_events
            ? t->unsubscribe_events(host_, plugin_, subscription) : FWGUI_ERR_UNSUPPORTED;
    }

private:
    const FwGuiCoreApi* Core() const { return HasCore() ? api_->core : nullptr; }
    const FwGuiUiApi* Ui() const { return HasUi() ? api_->ui : nullptr; }
    const FwGuiMcpApi* Mcp() const { return HasMcp() ? api_->mcp : nullptr; }
    const FwGuiFreeWiliApi* FreeWili() const { return HasFreeWili() ? api_->freewili : nullptr; }
    FwGuiHost* host_ = nullptr;
    const FwGuiHostApi* api_ = nullptr;
    FwGuiPlugin* plugin_ = nullptr;
};

struct View {
    const char* id = nullptr;
    const char* title = nullptr;
    const char* menu_group = nullptr;
    FwGuiViewFlags flags = 0;
    int32_t imgui_window_flags = 0;
    void* user = nullptr;
    const char* menu_path = nullptr;
};

class ViewCtx {
public:
    ViewCtx(const FwGuiViewCtx& ctx, Host host) : ctx_(&ctx), host_(host) {}
    const Host& host() const noexcept { return host_; }
    void* user() const noexcept { return ctx_->user; }
    double dt() const noexcept { return ctx_->dt; }
    uint64_t frame_index() const noexcept { return ctx_->frame_index; }
private:
    const FwGuiViewCtx* ctx_;
    Host host_;
};

class CanvasImage;

class Canvas {
public:
    Canvas() = default;
    Canvas(const Host& host, int32_t width, int32_t height) : host_(host) {
        const auto* t = Table();
        if (t && FWGUI_HAS(t, create) && t->create)
            value_ = t->create(host_.RawHost(), host_.Plugin(), width, height);
    }
    ~Canvas() { Reset(); }
    Canvas(const Canvas&) = delete;
    Canvas& operator=(const Canvas&) = delete;
    Canvas(Canvas&& other) noexcept { MoveFrom(other); }
    Canvas& operator=(Canvas&& other) noexcept {
        if (this != &other) { Reset(); MoveFrom(other); }
        return *this;
    }
    // Wraps a handle the host lends for one callback (panel-control paint).
    // The wrapper never destroys it.
    static Canvas Borrow(const Host& host, FwGuiCanvas* handle) {
        Canvas c; c.host_ = host; c.value_ = handle; c.owned_ = false; return c;
    }
    explicit operator bool() const noexcept { return value_ != nullptr; }
    FwGuiCanvas* Raw() const noexcept { return value_; }
    void Reset() {
        const auto* t = Table();
        if (value_ && owned_ && t && FWGUI_HAS(t, destroy) && t->destroy) t->destroy(host_.RawHost(), value_);
        value_ = nullptr;
    }
#define FWGUI_CANVAS_VOID0(Name, field) void Name() { const auto* t=Table(); if(value_&&t&&FWGUI_HAS(t,field)&&t->field)t->field(host_.RawHost(),value_); }
#define FWGUI_CANVAS_VOID1(Name, field, T1) void Name(T1 a) { const auto* t=Table(); if(value_&&t&&FWGUI_HAS(t,field)&&t->field)t->field(host_.RawHost(),value_,a); }
#define FWGUI_CANVAS_VOID2(Name, field, T1, T2) void Name(T1 a,T2 b) { const auto* t=Table(); if(value_&&t&&FWGUI_HAS(t,field)&&t->field)t->field(host_.RawHost(),value_,a,b); }
#define FWGUI_CANVAS_VOID4(Name, field) void Name(double a,double b,double c,double d) { const auto* t=Table(); if(value_&&t&&FWGUI_HAS(t,field)&&t->field)t->field(host_.RawHost(),value_,a,b,c,d); }
    void Resize(int32_t w,int32_t h){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,resize)&&t->resize)t->resize(host_.RawHost(),value_,w,h);}
    int32_t Width() const {const auto*t=Table();return value_&&t&&FWGUI_HAS(t,width)&&t->width?t->width(host_.RawHost(),value_):0;}
    int32_t Height() const {const auto*t=Table();return value_&&t&&FWGUI_HAS(t,height)&&t->height?t->height(host_.RawHost(),value_):0;}
    void Begin(uint32_t color){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,begin)&&t->begin)t->begin(host_.RawHost(),value_,color);}
    FWGUI_CANVAS_VOID0(End,end)
    uint64_t TextureId()const{const auto*t=Table();return value_&&t&&FWGUI_HAS(t,texture_id)&&t->texture_id?t->texture_id(host_.RawHost(),value_):0;}
#if defined(IMGUI_VERSION)
    // include <freewiligui/plugin.hpp> (or imgui.h) before host.hpp to get DrawImGui.
    void DrawImGui(const ImVec2& size=ImVec2(0,0))const{const uint64_t id=TextureId();if(id){const ImVec2 actual(size.x>0?size.x:static_cast<float>(Width()),size.y>0?size.y:static_cast<float>(Height()));ImGui::Image((ImTextureID)(intptr_t)id,actual);}}
#endif
    FWGUI_CANVAS_VOID0(Save,save) FWGUI_CANVAS_VOID0(Restore,restore)
    FWGUI_CANVAS_VOID1(SetGlobalAlpha,set_global_alpha,double)
    FWGUI_CANVAS_VOID1(SetFillStyleCss,set_fill_style_css,const char*)
    FWGUI_CANVAS_VOID1(SetFillStyle,set_fill_style,uint32_t)
    void SetFillStyleCorners(uint32_t a,uint32_t b,uint32_t c,uint32_t d){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,set_fill_style_corners)&&t->set_fill_style_corners)t->set_fill_style_corners(host_.RawHost(),value_,a,b,c,d);}
    FWGUI_CANVAS_VOID1(SetStrokeStyleCss,set_stroke_style_css,const char*)
    FWGUI_CANVAS_VOID1(SetStrokeStyle,set_stroke_style,uint32_t)
    FWGUI_CANVAS_VOID1(SetLineWidth,set_line_width,double)
    FWGUI_CANVAS_VOID1(SetLineCap,set_line_cap,int32_t)
    FWGUI_CANVAS_VOID1(SetLineJoin,set_line_join,int32_t)
    FWGUI_CANVAS_VOID4(FillRect,fill_rect) FWGUI_CANVAS_VOID4(StrokeRect,stroke_rect)
    FWGUI_CANVAS_VOID4(ClearRect,clear_rect)
    void FillStrokeRoundRect(double a,double b,double c,double d,double e){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,fill_stroke_round_rect)&&t->fill_stroke_round_rect)t->fill_stroke_round_rect(host_.RawHost(),value_,a,b,c,d,e);}
    FWGUI_CANVAS_VOID4(SetClipRect,set_clip_rect) FWGUI_CANVAS_VOID0(ClearClip,clear_clip)
    FWGUI_CANVAS_VOID0(BeginPath,begin_path) FWGUI_CANVAS_VOID0(ClosePath,close_path)
    FWGUI_CANVAS_VOID2(MoveTo,move_to,double,double) FWGUI_CANVAS_VOID2(LineTo,line_to,double,double)
    FWGUI_CANVAS_VOID4(Rect,rect)
    void Arc(double a,double b,double c,double d,double e,int32_t f=0){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,arc)&&t->arc)t->arc(host_.RawHost(),value_,a,b,c,d,e,f);}
    void ArcTo(double a,double b,double c,double d,double e){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,arc_to)&&t->arc_to)t->arc_to(host_.RawHost(),value_,a,b,c,d,e);}
    void QuadraticCurveTo(double a,double b,double c,double d){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,quadratic_curve_to)&&t->quadratic_curve_to)t->quadratic_curve_to(host_.RawHost(),value_,a,b,c,d);}
    void BezierCurveTo(double a,double b,double c,double d,double e,double f){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,bezier_curve_to)&&t->bezier_curve_to)t->bezier_curve_to(host_.RawHost(),value_,a,b,c,d,e,f);}
    FWGUI_CANVAS_VOID0(Fill,fill) FWGUI_CANVAS_VOID0(Stroke,stroke)
    void FillTriangle(double a,double b,double c,double d,double e,double f){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,fill_triangle)&&t->fill_triangle)t->fill_triangle(host_.RawHost(),value_,a,b,c,d,e,f);}
    FWGUI_CANVAS_VOID1(SetFontCss,set_font_css,const char*)
    void SetFont(double size,bool bold=false,bool italic=false){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,set_font)&&t->set_font)t->set_font(host_.RawHost(),value_,size,bold?1:0,italic?1:0);}
    FWGUI_CANVAS_VOID1(SetTextAlign,set_text_align,int32_t)
    FWGUI_CANVAS_VOID1(SetTextBaseline,set_text_baseline,int32_t)
    void FillText(const char* s,double x,double y){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,fill_text)&&t->fill_text)t->fill_text(host_.RawHost(),value_,s,x,y);}
    FwGuiCanvasTextMetrics MeasureText(const char* s)const{FwGuiCanvasTextMetrics m{};const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,measure_text)&&t->measure_text)t->measure_text(host_.RawHost(),value_,s,&m);return m;}
    void DrawImage(const CanvasImage& image,double x,double y);
    void DrawImage(const CanvasImage& image,double x,double y,double w,double h);
    void DrawImageRgba(const void*p,int32_t sw,int32_t sh,double x,double y,double w,double h){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,draw_image_rgba)&&t->draw_image_rgba)t->draw_image_rgba(host_.RawHost(),value_,p,sw,sh,x,y,w,h);}
    size_t ReadPixels(void* out,size_t capacity)const{const auto*t=Table();return value_&&t&&FWGUI_HAS(t,read_pixels)&&t->read_pixels?t->read_pixels(host_.RawHost(),value_,out,capacity):0;}
    uint32_t PixelAt(int32_t x,int32_t y)const{const auto*t=Table();return value_&&t&&FWGUI_HAS(t,pixel_at)&&t->pixel_at?t->pixel_at(host_.RawHost(),value_,x,y):0;}
    void SetLineDash(const double*p,int32_t n){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,set_line_dash)&&t->set_line_dash)t->set_line_dash(host_.RawHost(),value_,p,n);}
    FWGUI_CANVAS_VOID1(SetLineDashOffset,set_line_dash_offset,double)
    FWGUI_CANVAS_VOID2(Translate,translate,double,double)
    FWGUI_CANVAS_VOID1(Rotate,rotate,double)
    FWGUI_CANVAS_VOID2(Scale,scale,double,double)
    void SetTransform(double a,double b,double c,double d,double e,double f){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,set_transform)&&t->set_transform)t->set_transform(host_.RawHost(),value_,a,b,c,d,e,f);}
    FWGUI_CANVAS_VOID0(ResetTransform,reset_transform)
#undef FWGUI_CANVAS_VOID0
#undef FWGUI_CANVAS_VOID1
#undef FWGUI_CANVAS_VOID2
#undef FWGUI_CANVAS_VOID4
private:
    friend class CanvasImage;
    const FwGuiCanvasApi* Table() const { return host_.HasCanvas() ? host_.Api()->canvas : nullptr; }
    void MoveFrom(Canvas& other){host_=other.host_;value_=other.value_;owned_=other.owned_;other.value_=nullptr;}
    Host host_;
    FwGuiCanvas* value_ = nullptr;
    bool owned_ = true;
};

class CanvasImage {
public:
    CanvasImage() = default;
    static CanvasImage FromFile(const Host& h,const char* path){const auto*t=h.HasCanvas()?h.Api()->canvas:nullptr;return CanvasImage(h,t&&FWGUI_HAS(t,image_load_file)&&t->image_load_file?t->image_load_file(h.RawHost(),h.Plugin(),path):nullptr);}
    static CanvasImage FromMemory(const Host& h,const void*p,size_t n){const auto*t=h.HasCanvas()?h.Api()->canvas:nullptr;return CanvasImage(h,t&&FWGUI_HAS(t,image_load_memory)&&t->image_load_memory?t->image_load_memory(h.RawHost(),h.Plugin(),p,n):nullptr);}
    static CanvasImage FromPixels(const Host& h,const void*p,int32_t w,int32_t height){const auto*t=h.HasCanvas()?h.Api()->canvas:nullptr;return CanvasImage(h,t&&FWGUI_HAS(t,image_load_pixels)&&t->image_load_pixels?t->image_load_pixels(h.RawHost(),h.Plugin(),p,w,height):nullptr);}
    ~CanvasImage(){Reset();}
    CanvasImage(const CanvasImage&)=delete; CanvasImage& operator=(const CanvasImage&)=delete;
    CanvasImage(CanvasImage&&o)noexcept{MoveFrom(o);} CanvasImage&operator=(CanvasImage&&o)noexcept{if(this!=&o){Reset();MoveFrom(o);}return*this;}
    explicit operator bool()const noexcept{return value_!=nullptr;}
    FwGuiCanvasImage* Raw()const noexcept{return value_;}
    int32_t Width()const{const auto*t=Table();return value_&&t&&FWGUI_HAS(t,image_width)&&t->image_width?t->image_width(host_.RawHost(),value_):0;}
    int32_t Height()const{const auto*t=Table();return value_&&t&&FWGUI_HAS(t,image_height)&&t->image_height?t->image_height(host_.RawHost(),value_):0;}
    void Reset(){const auto*t=Table();if(value_&&t&&FWGUI_HAS(t,image_destroy)&&t->image_destroy)t->image_destroy(host_.RawHost(),value_);value_=nullptr;}
private:
    CanvasImage(const Host&h,FwGuiCanvasImage*v):host_(h),value_(v){}
    const FwGuiCanvasApi*Table()const{return host_.HasCanvas()?host_.Api()->canvas:nullptr;}
    void MoveFrom(CanvasImage&o){host_=o.host_;value_=o.value_;o.value_=nullptr;}
    Host host_; FwGuiCanvasImage* value_=nullptr;
};

inline void Canvas::DrawImage(const CanvasImage&i,double x,double y){const auto*t=Table();if(value_&&i.Raw()&&t&&FWGUI_HAS(t,draw_image)&&t->draw_image)t->draw_image(host_.RawHost(),value_,i.Raw(),x,y);}
inline void Canvas::DrawImage(const CanvasImage&i,double x,double y,double w,double h){const auto*t=Table();if(value_&&i.Raw()&&t&&FWGUI_HAS(t,draw_image_scaled)&&t->draw_image_scaled)t->draw_image_scaled(host_.RawHost(),value_,i.Raw(),x,y,w,h);}

namespace detail {
template<class T,class=void>struct HasFrame:std::false_type{};
template<class T>struct HasFrame<T,std::void_t<decltype(std::declval<T&>().OnFrame(std::declval<const FwGuiFrameInfo&>()))>>:std::true_type{};
template<class T,class=void>struct HasEvent:std::false_type{};
template<class T>struct HasEvent<T,std::void_t<decltype(std::declval<T&>().OnEvent(FwGuiEvent{},std::declval<const FwGuiEventData*>()))>>:std::true_type{};
template<class T,class=void>struct HasShutdown:std::false_type{};
template<class T>struct HasShutdown<T,std::void_t<decltype(std::declval<T&>().OnShutdown())>>:std::true_type{};
}

namespace detail { struct NoUi { static void Bind(const FwGuiUiApi*, FwGuiHost*) noexcept {} }; }

template<class Derived, class UiBinder>
class PluginBase {
public:
    template<void (Derived::*Method)(const ViewCtx&)>
    FwGuiResult RegisterView(const View& view) {
        const auto* ui=host_.Api()&&FWGUI_HAS(host_.Api(),ui)?host_.Api()->ui:nullptr;
        if(!ui||!FWGUI_HAS(ui,register_view)||!ui->register_view)return FWGUI_ERR_UNSUPPORTED;
        FwGuiViewDesc d{sizeof(FwGuiViewDesc),view.id,view.title,view.menu_group,view.flags,
                        view.imgui_window_flags,&DrawThunk<Method>,view.user,view.menu_path};
        return ui->register_view(host_.RawHost(),host_.Plugin(),&d);
    }
    FwGuiResult RegisterDoc(const char* id,const char* title,const char* markdown) {
        const auto* ui=host_.Api()&&FWGUI_HAS(host_.Api(),ui)?host_.Api()->ui:nullptr;
        if(!ui||!FWGUI_HAS(ui,register_doc)||!ui->register_doc)return FWGUI_ERR_UNSUPPORTED;
        FwGuiDocDesc d{sizeof(FwGuiDocDesc),id,title,markdown};
        return ui->register_doc(host_.RawHost(),host_.Plugin(),&d);
    }
    template<void (Derived::*Method)(std::string_view,const McpReply&)>
    FwGuiResult RegisterTool(const char* name,const char* description,const char* schema,void* user=nullptr){
        const auto*m=host_.Api()&&FWGUI_HAS(host_.Api(),mcp)?host_.Api()->mcp:nullptr;
        if(!m||!FWGUI_HAS(m,register_tool)||!m->register_tool)return FWGUI_ERR_UNSUPPORTED;
        return m->register_tool(host_.RawHost(),host_.Plugin(),name,description,schema,&McpThunk<Method>,user);
    }
    static FwGuiPlugin* Create() noexcept {try{return reinterpret_cast<FwGuiPlugin*>(new Derived());}catch(...){return nullptr;}}
    static const FwGuiPluginVTable* VTable(){static const FwGuiPluginVTable t=MakeVTable();return&t;}
protected:
    const Host& GetHost()const{return host_;}
private:
    template<void (Derived::*Method)(const ViewCtx&)>
    static void DrawThunk(FwGuiPlugin*p,const FwGuiViewCtx*c){auto*self=reinterpret_cast<Derived*>(p);try{(self->*Method)(ViewCtx(*c,self->host_));}catch(...){self->LogCallbackFailure("plugin view callback threw an exception");}}
    template<void (Derived::*Method)(std::string_view,const McpReply&)>
    static void McpThunk(FwGuiPlugin*p,const char*a,FwGuiMcpReply*r,void*){auto*self=reinterpret_cast<Derived*>(p);McpReply reply(self->host_.Api()->mcp,r);try{(self->*Method)(a?std::string_view(a):std::string_view(),reply);}catch(...){if(!reply.HasReplied())reply.Error("plugin tool callback threw an exception");}}
    static FwGuiResult StartupThunk(FwGuiPlugin*p,FwGuiHost*h,const FwGuiHostApi*a){
        auto*self=reinterpret_cast<Derived*>(p);self->host_=Host(h,a,p);
        const auto*ui=a&&FWGUI_HAS(a,ui)?a->ui:nullptr;
        UiBinder::Bind(ui,h);
        FwGuiResult result=FWGUI_ERR_FAILED;
        try {
            result=self->OnStartup(self->host_);
        } catch (...) {
            self->host_.LogError("plugin startup threw an exception");
        }
        if(result!=FWGUI_OK)delete self;
        return result;
    }
    static FwGuiPluginVTable MakeVTable(){FwGuiPluginVTable t{sizeof(t),&StartupThunk,&ShutdownThunk,nullptr,nullptr};if constexpr(detail::HasFrame<Derived>::value)t.frame=&FrameThunk;if constexpr(detail::HasEvent<Derived>::value)t.event=&EventThunk;return t;}
    static void ShutdownThunk(FwGuiPlugin*p){auto*self=reinterpret_cast<Derived*>(p);if constexpr(detail::HasShutdown<Derived>::value){try{self->OnShutdown();}catch(...){self->host_.LogError("plugin shutdown threw an exception");}}delete self;}
    static void FrameThunk(FwGuiPlugin*p,const FwGuiFrameInfo*i){if constexpr(detail::HasFrame<Derived>::value){auto*self=reinterpret_cast<Derived*>(p);try{self->OnFrame(*i);}catch(...){self->LogCallbackFailure("plugin frame callback threw an exception");}}}
    static void EventThunk(FwGuiPlugin*p,FwGuiEvent e,const FwGuiEventData*d){if constexpr(detail::HasEvent<Derived>::value){auto*self=reinterpret_cast<Derived*>(p);try{self->OnEvent(e,d);}catch(...){self->LogCallbackFailure("plugin event callback threw an exception");}}}
    void LogCallbackFailure(const char* message) noexcept {try{host_.LogError(message);}catch(...){}}
    Host host_;
};

}
#endif
