# Quick start

Install the SDK and configure a copy of plugin-template:

~~~sh
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/path/to/sdk-install
cmake --build build
cmake --install build
cmake -S plugin-template -B plugin-build -DCMAKE_PREFIX_PATH=/path/to/sdk-install
cmake --build plugin-build
~~~

The helper requires ID, SYMBOL, and SOURCES. It accepts
PLUGIN_OUTPUT_DIRECTORY and OUTPUT_NAME. Each target also has a
FREEWILIGUI_PLUGIN_<TARGET> cache choice: SHARED, STATIC, or OFF.

Standalone builds fetch Dear ImGui v1.92.8-docking and ImPlot v1.0. Set
FREEWILIGUI_IMGUI_DIR and FREEWILIGUI_IMPLOT_DIR to local source trees, or set
FREEWILIGUI_IMGUI_TARGET and FREEWILIGUI_IMPLOT_TARGET to existing targets.
Use FREEWILIGUI_JSON_TARGET to build the dashboard with an existing nlohmann
JSON target.
