#include <stddef.h>
#include <freewiligui/host_api.h>

#define ASSERT_AT_COMPILE(name, condition) typedef char name[(condition) ? 1 : -1]
ASSERT_AT_COMPILE(root_order_1, offsetof(FwGuiHostApi, core) < offsetof(FwGuiHostApi, ui));
ASSERT_AT_COMPILE(root_order_2, offsetof(FwGuiHostApi, ui) < offsetof(FwGuiHostApi, canvas));
ASSERT_AT_COMPILE(root_order_3, offsetof(FwGuiHostApi, canvas) < offsetof(FwGuiHostApi, mcp));
ASSERT_AT_COMPILE(root_order_4, offsetof(FwGuiHostApi, mcp) < offsetof(FwGuiHostApi, freewili));
ASSERT_AT_COMPILE(frame_order, offsetof(FwGuiFrameInfo, host_time) < offsetof(FwGuiFrameInfo, frame_index));
ASSERT_AT_COMPILE(view_context_is_portable,
    sizeof(FwGuiViewCtx) >= offsetof(FwGuiViewCtx, frame_index) + sizeof(uint64_t));
ASSERT_AT_COMPILE(canvas_has_61_callbacks,
    (offsetof(FwGuiCanvasApi, reset_transform) - offsetof(FwGuiCanvasApi, create))
        / sizeof(void*) + 1 == 61);

int main(void) {
    FwGuiHostApi api = {0};
    FwGuiFrameInfo frame = {0};
    FwGuiViewCtx view = {0};
    return (FWGUI_SDK_VERSION_MAJOR == 0 && FWGUI_ABI_VERSION == 1 &&
            offsetof(FwGuiHostApi, core) < offsetof(FwGuiHostApi, freewili) &&
            offsetof(FwGuiFrameInfo, dt) < offsetof(FwGuiFrameInfo, frame_index) &&
            offsetof(FwGuiViewCtx, host) < offsetof(FwGuiViewCtx, frame_index) &&
            api.struct_size == 0 && frame.struct_size == 0 && view.struct_size == 0) ? 0 : 1;
}
