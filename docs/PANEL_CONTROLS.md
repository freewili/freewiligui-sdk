# Panel controls

## What a panel control is

A panel control is a native plugin that adds a control to the Graphical Panel
(GPanel) editor. It appears in the palette, and is placed, resized, edited in
the properties grid, saved, copied and undone like a built-in control. Panel
controls run in the desktop application only; the web build shows a placeholder.

A control needs only the SDK headers. It does not link ImGui or ImPlot, so no
matching ImGui build, runtime library, or Debug/Release choice is needed.

## Quick start

Copy `plugin-template-control` into your own project. Set the ID in
`CMakeLists.txt` and in the descriptor in `src/control.cpp` (they must match),
then build against an installed SDK:

~~~sh
cmake -S plugin-template-control -B control-build -DCMAKE_PREFIX_PATH="$PWD/sdk-install"
cmake --build control-build --config Release
~~~

Install the library into `plugins/` beside the application executable, into
`plugins/` in the application's data directory, or into a directory named by
`FWCOM_PLUGIN_DIR`. Restart the application and find the control in the GPanel
palette, in its declared section or under **Plugins**. The full example is
[examples/example_panel_control](../examples/example_panel_control), a slider.

A control type has the full key `<plugin id>/<type id>`. A type id may not
contain `=` or `/`, and a plugin may register up to 64 types of up to 32
properties each. Register types in `OnStartup` with
`fwgui::RegisterControlType<T>`.

## Lifecycle

All callbacks run on the UI thread.

| Callback | When it runs |
|---|---|
| `create` | A control is placed, loaded, pasted, restored by undo, or its type becomes available again. Returns your per-instance state; it may return NULL for a stateless control, which is still live. In C++, a constructor that throws leaves the control created blank and the error is logged. |
| `paint` | Every frame the control is visible, including while editing. |
| `input` | Mouse, wheel, key, character and focus events, in run mode only. Return 1 when you used the event. |
| `property_changed` | After the host stores a new value for a property edited in the grid or set with `set_property`. It is not called when a control is loaded. A `set_property` made inside a callback is delivered after that callback returns. Optional. |
| `destroy` | The control is deleted, the panel or project is closed, the type is unregistered, or the plugin stops. |

`destroy` is not called for a plugin that has faulted; its instances are
abandoned and shown as placeholders.

## Properties

You declare each property with a name (`A-Za-z0-9_`, 1 to 64 characters, unique
ignoring case), a kind, and a default value written as text. The host stores
every value as text, saves it in the panel file, and shows it in the grid.

| Kind | Grid editor | Text value | `get_number` |
|---|---|---|---|
| `FWGUI_PROP_BOOL` | Checkbox | `1` or `0` | 1 or 0 |
| `FWGUI_PROP_INT` | Number, with min/max | Decimal integer | The value |
| `FWGUI_PROP_NUMBER` | Number, with min/max | Decimal number | The value |
| `FWGUI_PROP_STRING` | Text | The string | 0 |
| `FWGUI_PROP_COLOR` | Color | `#rrggbb` | Packed `0xAARRGGBB` |
| `FWGUI_PROP_LIST` | Drop-down of `list_items` | The selected label | Zero-based index |
| `FWGUI_PROP_FILE` | File path | The path | 0 |
| `FWGUI_PROP_SIGNAL_READ` | Signal picker (read) | A signal or `=` expression | 0 |
| `FWGUI_PROP_SIGNAL_WRITE` | Signal picker (write) | A signal name | 0 |

For INT and NUMBER, `min == max` means unbounded. `list_items` is a
comma-separated list of labels. `get_string` returns a view that is valid until
the next callback returns; copy it if you need it longer.

`set_property` changes a value at run time, for example when a slider moves.
It calls `property_changed` and marks the panel modified, but it is not an
undo step. A property that a saved file holds and your schema no longer
declares is kept and saved again, so upgrading or downgrading a plugin does
not lose data.

## Signals

Declare a property as `FWGUI_PROP_SIGNAL_READ` or `FWGUI_PROP_SIGNAL_WRITE` and
the grid offers the same signal picker as built-in controls. In C++ use
`Control::ReadNumber`, `ReadText`, `Write` and `WriteText`; in C call
`read_signal` and `write_signal`.

- `read_signal` evaluates the property's binding. App signals, `=` rThon
  expressions and device providers work as they do for built-in controls.
- `write_signal` sends numbers only. A text value is accepted only if it parses as a number.
- An empty binding returns `FWGUI_ERR_NOT_FOUND`.

## Drawing

`paint` receives a **borrowed** canvas, valid only during the call. In C++,
`fwgui::Canvas&` wraps it. Draw with the normal Canvas calls (see
[Canvas](CANVAS.md)).

- Coordinates are control-local panel units: (0, 0) is the control's top-left
  and `info.width`, `info.height` its bottom-right. The host has already applied
  panel zoom, pan, clipping and layer dimming.
- The host saves and restores canvas state around each paint, so transforms,
  clips, alpha and styles you set do not affect other controls.
- Lifecycle calls (`create`, `destroy`, `resize`, `begin`, `end`,
  `texture_id`, `read_pixels`, `pixel_at`) fail on a borrowed handle.

The GPanel canvas has limits. Draw with rectangles, rounded rectangles,
stroked paths, lines and text:

- Filled arcs and `fill_triangle` may not render on the GPU canvas.
- On the GPanel canvas, filled paths (`fill()`), `fill_triangle`, rotated or
  skewed `fill_rect` and `draw_image_rgba` may draw nothing. Use axis-aligned
  `fill_rect`, `fill_stroke_round_rect`, strokes and text.
- `arc_to` is not drawn.
- There is one font. `SetFont` changes size and style only.
- The clip is a rectangle.
- There are no linear or radial gradients, except the four-corner fill.

## Input and focus

Input is delivered only when the panel is in run mode (locked). Mouse
coordinates are control-local. A control that receives `MOUSE_DOWN` also gets
the matching `MOUSE_UP`. Call `request_focus` to take keyboard focus; key and
character events go only to the focused control. Key codes are Windows
virtual-key values on every platform (`FWGUI_KEY_LEFT`, `FWGUI_KEY_ENTER`, ...);
text arrives as `FWGUI_INPUT_CHAR` with a UTF-32 code point. While editing,
`paint` still runs with `FWGUI_PAINT_EDITING` set, and the host draws the
selection handles.

## Threading

Every callback runs on the UI thread. Helper calls made from any other thread
fail with `FWGUI_ERR_WRONG_THREAD`. Do not start threads that touch the host or a
`fwgui::Control`; hand data to the UI thread through your own state.

## Missing plugins

A panel opens safely without your plugin. Each control of a missing type is a
placeholder with a dashed outline, the type label, and one line of status:

- Plugin `<id>` is not installed
- Plugin `<id>` is disabled
- Plugin `<id>` stopped after an error
- Plugin controls need the desktop app

A placeholder keeps all its properties; they can be edited as text, and saving
writes them back unchanged. When the plugin is installed or enabled again, the
placeholders become live with their saved values. A control that faults in
`paint` or `input` is counted against the plugin's fault limit, and a crash
disables the plugin at once; the application keeps running.

## ImGui-free builds

A panel control needs no ImGui. Declare that in the descriptor:

- Set the descriptor's `flags` to `FWGUI_PLUGIN_NO_IMGUI`.
- Use `FWGUI_FINGERPRINT_INIT_NO_IMGUI` for the fingerprint.
- In CMake, use `freewiligui_add_plugin(name NO_IMGUI ...)`, which does not
  fetch or link ImGui or ImPlot.
- In C++ include `freewiligui/panel_control.hpp`, derive from
  `fwgui::ControlPlugin`, and use `FWGUI_PLUGIN_MAIN_NO_IMGUI`.

The host then skips the ImGui, ImPlot, CRT and Debug/Release fingerprint
checks, so no ImGui, ImPlot or CRT match is needed. The ABI version check still
applies. Such a plugin cannot register views (`api->ui` is NULL); it keeps the
core, canvas, agent-tool, FreeWili and panel-control families.

## Upgrading from 0.1

- `fwgui::Plugin<D>` is now an alias template of `fwgui::PluginBase<D, ImGuiBinder>`.
  Code that forward-declares `template<class> class Plugin` or befriends
  `fwgui::Plugin<D>` must be updated (use `PluginBase`).
- `Canvas::DrawImGui` needs `imgui.h` (or `freewiligui/plugin.hpp`) included
  before `freewiligui/host.hpp`.
