# Panel control template

A minimal Graphical Panel control: one `value` property, drawn as text.
Change the ID in `CMakeLists.txt` and in `src/control.cpp` (they must match), then
configure with `CMAKE_PREFIX_PATH` pointing at an installed FreeWili GUI SDK (0.2 or newer)
and build with `cmake --build <dir> --config Release`.
Copy the library from `<dir>/plugins/` to the `plugins/` folder beside FreeWili GUI, restart it,
and find **My Control** under **Plugins** in the Graphical Panel palette.
See [PANEL_CONTROLS.md](../docs/PANEL_CONTROLS.md).
