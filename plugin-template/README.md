# Native plugin template

A minimal dockable view. Copy this folder, then:

1. Set the plugin ID in `CMakeLists.txt` (`ID`) and in the descriptor in
   `src/plugin.cpp`; they must match. Fill in the name, version, author and
   description: every one of them is required (only the homepage is optional).
2. Configure with `CMAKE_PREFIX_PATH` pointing at an installed FreeWili GUI
   SDK 0.3 or newer, using the same generator, architecture and configuration
   as the FreeWili GUI you will load it into, and build.
3. Replace the picture and the help page (below) before you ship.

## The picture and the help page are required

FreeWili GUI's plugin browser (**Setup > Manage plugins**) shows a picture
and an Overview page for every plugin, read from the library itself before
the plugin starts. A plugin that does not embed them does not load. The
`freewiligui_browser_assets()` call in `CMakeLists.txt` embeds:

- `assets/preview.png`: the picture. The shipped file is a placeholder so
  the first build loads; replace it with a real screenshot of the finished
  view. An agent working inside FreeWili GUI gets exactly that PNG from the
  application's `view_screenshot` MCP tool. Limits: PNG, 1 to 4096 pixels per
  side, at most 16 MiB. A 4:3 or 16:10 picture around 320 by 200 looks right
  in the browser.
- `help.md`: the Overview, Markdown, at most 1 MiB, not blank.

Both files are configure dependencies: edit them and rebuild, no CMake
re-run needed. The build fails with a clear message if either is missing,
blank, or not a valid PNG.
