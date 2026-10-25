# Panel slider example

A slider control for Graphical Panels. It has a `value` (0-100), a `binding`
signal it writes when moved, an optional `source` signal it follows, and
`track_color` / `knob_color`. Drag, mouse wheel, and arrow keys change it; a focus ring
shows when it has keyboard focus. It draws only rectangles, rounded rectangles, and text,
which every GPanel canvas supports, and does not link ImGui.

Build with `-DFREEWILIGUI_BUILD_EXAMPLES=ON` (see the [quick start](../../docs/QUICKSTART.md)),
copy `example_panel_control` from `plugins/` to FreeWili GUI's `plugins/` folder, restart,
and pick **Slider** from the **Instruments** palette section.
See [PANEL_CONTROLS.md](../../docs/PANEL_CONTROLS.md).
