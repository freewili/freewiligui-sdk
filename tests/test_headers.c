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
ASSERT_AT_COMPILE(root_order_5, offsetof(FwGuiHostApi, freewili) < offsetof(FwGuiHostApi, panel_control));
ASSERT_AT_COMPILE(desc_flags_appended, offsetof(FwGuiPluginDesc, build) < offsetof(FwGuiPluginDesc, flags));
ASSERT_AT_COMPILE(panel_api_order_1, offsetof(FwGuiPanelControlApi, register_type) < offsetof(FwGuiPanelControlApi, unregister_type));
ASSERT_AT_COMPILE(panel_api_order_2, offsetof(FwGuiPanelControlApi, unregister_type) < offsetof(FwGuiPanelControlApi, get_number));
ASSERT_AT_COMPILE(panel_api_order_3, offsetof(FwGuiPanelControlApi, get_number) < offsetof(FwGuiPanelControlApi, get_string));
ASSERT_AT_COMPILE(panel_api_order_4, offsetof(FwGuiPanelControlApi, get_string) < offsetof(FwGuiPanelControlApi, set_property));
ASSERT_AT_COMPILE(panel_api_order_5, offsetof(FwGuiPanelControlApi, set_property) < offsetof(FwGuiPanelControlApi, read_signal));
ASSERT_AT_COMPILE(panel_api_order_6, offsetof(FwGuiPanelControlApi, read_signal) < offsetof(FwGuiPanelControlApi, write_signal));
ASSERT_AT_COMPILE(panel_api_order_7, offsetof(FwGuiPanelControlApi, write_signal) < offsetof(FwGuiPanelControlApi, request_focus));
ASSERT_AT_COMPILE(panel_api_order_8, offsetof(FwGuiPanelControlApi, request_focus) < offsetof(FwGuiPanelControlApi, release_focus));
ASSERT_AT_COMPILE(callbacks_order, offsetof(FwGuiControlCallbacks, paint) < offsetof(FwGuiControlCallbacks, property_changed));
/* FwGuiControlCallbacks is embedded by value in FwGuiControlTypeDesc, so its
 * layout is frozen at SDK 0.2: a struct_size word then five function pointers.
 * New callbacks are trailing fields of FwGuiControlTypeDesc, after type_user. */
#define FWGUI_TEST_FN_SIZE sizeof(((FwGuiControlCallbacks*)0)->create)
ASSERT_AT_COMPILE(callbacks_frozen_create, offsetof(FwGuiControlCallbacks, create) == FWGUI_TEST_FN_SIZE);
ASSERT_AT_COMPILE(callbacks_frozen_destroy, offsetof(FwGuiControlCallbacks, destroy) == 2 * FWGUI_TEST_FN_SIZE);
ASSERT_AT_COMPILE(callbacks_frozen_paint, offsetof(FwGuiControlCallbacks, paint) == 3 * FWGUI_TEST_FN_SIZE);
ASSERT_AT_COMPILE(callbacks_frozen_input, offsetof(FwGuiControlCallbacks, input) == 4 * FWGUI_TEST_FN_SIZE);
ASSERT_AT_COMPILE(callbacks_frozen_property_changed,
    offsetof(FwGuiControlCallbacks, property_changed) == 5 * FWGUI_TEST_FN_SIZE);
ASSERT_AT_COMPILE(callbacks_frozen_size, sizeof(FwGuiControlCallbacks) == 6 * FWGUI_TEST_FN_SIZE);
ASSERT_AT_COMPILE(type_desc_callbacks_after_properties,
    offsetof(FwGuiControlTypeDesc, property_count) < offsetof(FwGuiControlTypeDesc, callbacks));
ASSERT_AT_COMPILE(type_desc_type_user_follows_callbacks,
    offsetof(FwGuiControlTypeDesc, type_user) ==
        offsetof(FwGuiControlTypeDesc, callbacks) + sizeof(FwGuiControlCallbacks));
ASSERT_AT_COMPILE(prop_kind_values, FWGUI_PROP_BOOL == 1 && FWGUI_PROP_SIGNAL_WRITE == 9);
ASSERT_AT_COMPILE(key_values_are_virtual_keys, FWGUI_KEY_LEFT == 0x25 && FWGUI_KEY_ENTER == 0x0D);

int main(void) {
    FwGuiHostApi api = {0};
    FwGuiFrameInfo frame = {0};
    FwGuiViewCtx view = {0};
    return (FWGUI_SDK_VERSION_MAJOR == 0 && FWGUI_SDK_VERSION_MINOR == 2 &&
            FWGUI_HAS(&api, panel_control) == 0 && FWGUI_ABI_VERSION == 1 &&
            offsetof(FwGuiHostApi, core) < offsetof(FwGuiHostApi, freewili) &&
            offsetof(FwGuiFrameInfo, dt) < offsetof(FwGuiFrameInfo, frame_index) &&
            offsetof(FwGuiViewCtx, host) < offsetof(FwGuiViewCtx, frame_index) &&
            api.struct_size == 0 && frame.struct_size == 0 && view.struct_size == 0) ? 0 : 1;
}
