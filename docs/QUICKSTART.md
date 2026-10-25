# Quick start

## Build and install the SDK

Install Git, CMake 3.21 or newer, and C99/C++17 compilers. On Windows, install
Visual Studio with the **Desktop development with C++** workload. Select the
compiler toolset and architecture that match your FreeWili GUI build.

From the SDK checkout:

~~~sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix "$PWD/sdk-install"
~~~

For a Windows x64 application built with Visual Studio 2026, configure with
`-G "Visual Studio 18 2026" -A x64`. Use the same generator and architecture
when configuring the plugin below. With a multi-configuration generator,
`--config Release` selects the build configuration; `CMAKE_BUILD_TYPE` is
used by single-configuration generators such as Ninja and Unix Makefiles.

## Create a plugin

Copy `plugin-template` into your own project, or build it directly to try it:

~~~sh
cmake -S plugin-template -B plugin-build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PWD/sdk-install"
cmake --build plugin-build --config Release
~~~

`$PWD` is the SDK checkout directory in these examples and works in PowerShell
and POSIX shells. For a separate project, replace the install path with the
absolute path to `sdk-install`.

Set your plugin's unique ID in both `CMakeLists.txt` and the descriptor in
`src/plugin.cpp`. Set its display name, version, author and description in
the descriptor; all of them are required, only the homepage is optional.

### The picture and the help page are required

FreeWili GUI's plugin browser (**Setup > Manage plugins**) shows a picture,
a type and an Overview page for every plugin, read from the library before
the plugin starts. Since SDK 0.3.0 a plugin that does not embed them does
not load. The template's `CMakeLists.txt` embeds them with

~~~cmake
freewiligui_browser_assets(my_plugin KINDS VIEW
    THUMBNAIL assets/preview.png
    HELP help.md)
~~~

and the descriptor in `src/plugin.cpp` includes the generated
`freewiligui_browser.h` and ends with `0, FWGUI_BROWSER_INFO`.

- `assets/preview.png` is the picture. The template ships a placeholder so
  the first build loads; replace it with a real screenshot of the finished
  view before you ship. An agent working inside FreeWili GUI gets exactly
  that PNG from the application's `view_screenshot` MCP tool. Limits: PNG,
  1 to 4096 pixels per side, at most 16 MiB.
- `help.md` is the Overview page: Markdown, at most 1 MiB, not blank.
  Rewrite the template's text to describe your view.

`KINDS` lists what the plugin supplies: `VIEW`, `PANEL_CONTROL` and/or
`MCP_TOOL`. Both files are configure dependencies, so editing them and
rebuilding is enough. A missing, blank or invalid file fails the configure
step with a message naming the problem. The full contract is in
[BROWSER_CONTRACT.md](BROWSER_CONTRACT.md).

The helper requires ID, SYMBOL, and SOURCES. It accepts
PLUGIN_OUTPUT_DIRECTORY and OUTPUT_NAME. Each target also has a
FREEWILIGUI_PLUGIN_<TARGET> cache choice: SHARED, STATIC, or OFF. The target
name is uppercased, with punctuation replaced by underscores; for the
template this is `FREEWILIGUI_PLUGIN_MY_PLUGIN`. SHARED is the default.

## Match the application's build

Native plugins must match the application's architecture, ABI, compiler,
Debug/Release configuration, runtime library, and Dear ImGui/ImPlot layout.
The loader checks the plugin's build fingerprint before starting it.

Standalone builds fetch Dear ImGui v1.92.8-docking and ImPlot v1.0. Set
FREEWILIGUI_IMGUI_DIR and FREEWILIGUI_IMPLOT_DIR to local source trees, or set
FREEWILIGUI_IMGUI_TARGET and FREEWILIGUI_IMPLOT_TARGET to existing targets.
Use FREEWILIGUI_JSON_TARGET to build the dashboard with an existing nlohmann
JSON target.

`FREEWILIGUI_IMGUI_TEST_ENGINE` defaults to ON and must match the application.
On Windows, `FREEWILIGUI_PLUGIN_MSVC_RUNTIME` defaults to the static runtime
(`/MT` for Release and `/MTd` for Debug). Override these only to match the
application's build. See [API and ownership](API.md) for callback and memory
lifetime rules.

## Install and open the plugin

The template writes its library to `plugin-build/plugins/`, or
`plugin-build/plugins/Release/` with a multi-configuration generator. On
Windows, the file is `my_plugin.dll`.

1. Copy the library into `plugins/` beside the FreeWili GUI executable, or
   into the application's data directory under `plugins/`.
2. Restart FreeWili GUI, or use **Reload** in **Setup > Manage plugins**,
   and check the plugin's status there. The browser shows your picture
   beside the title and your help page on the Help tab. Enable/disable
   changes take effect immediately.
3. Open the registered view from the application's menus.

For development, `FWCOM_PLUGIN_DIR` can point to an additional plugin
directory. If loading reports a fingerprint mismatch, rebuild with settings
that match the application.

## Build the examples

~~~sh
cmake -S . -B example-build -DCMAKE_BUILD_TYPE=Release -DFREEWILIGUI_BUILD_EXAMPLES=ON
cmake --build example-build --config Release
ctest --test-dir example-build -C Release --output-on-failure
~~~

The libraries appear under `example-build/plugins/` (with a configuration
subdirectory when applicable). Install them the same way as the template.

- [Canvas example](../examples/example_canvas): a moving gauge drawn with
  the native Canvas API.
- [FreeWili dashboard](../examples/freewili_dashboard): device discovery,
  asynchronous jobs, cancellation, and an agent-readable status tool. This
  example also fetches JSON for Modern C++ from its public repository.
- [Panel slider](../examples/example_panel_control): a Graphical Panel control with no ImGui dependency. To write your own, start from `plugin-template-control` and read [panel controls](PANEL_CONTROLS.md).
