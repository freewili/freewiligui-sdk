# Panel control template

A minimal Graphical Panel control: one `value` property, drawn as text.

1. Change the ID in `CMakeLists.txt` and in `src/control.cpp` (they must
   match) and fill in the name, version, author and description; every one
   of them is required (only the homepage is optional).
2. Configure with `CMAKE_PREFIX_PATH` pointing at an installed FreeWili GUI
   SDK (0.3 or newer) and build with `cmake --build <dir> --config Release`.
3. Copy the library from `<dir>/plugins/` to the `plugins/` folder beside
   FreeWili GUI, restart it (or use **Reload** in **Setup > Manage plugins**),
   and find **My Control** under **Plugins** in the Graphical Panel palette.

See [PANEL_CONTROLS.md](../docs/PANEL_CONTROLS.md).

## The picture and the help page are required

FreeWili GUI's plugin browser shows a picture and an Overview page for
every plugin, read from the library before the plugin starts; a plugin
without them does not load. `freewiligui_browser_assets()` in
`CMakeLists.txt` embeds:

- `assets/preview.png`: the picture. The shipped file is a placeholder so
  the first build loads; replace it with a picture of the finished control
  on a panel. PNG, 1 to 4096 pixels per side, at most 16 MiB.
- `help.md`: the Overview, Markdown, at most 1 MiB, not blank.

Both are configure dependencies: edit and rebuild. The build fails with a
clear message if either is missing, blank, or not a valid PNG.
