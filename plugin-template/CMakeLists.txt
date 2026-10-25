cmake_minimum_required(VERSION 3.21)
project(freewili_plugin LANGUAGES CXX)
find_package(freewiligui 0.3 CONFIG REQUIRED)
freewiligui_add_plugin(my_plugin
    ID org.example.my-plugin
    SYMBOL my_plugin
    SOURCES src/plugin.cpp
    PLUGIN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
# Required browser metadata: the picture and help page FreeWili GUI shows
# in Setup > Manage plugins. assets/preview.png is a placeholder so the
# first build loads; replace it with a screenshot of the finished view.
freewiligui_browser_assets(my_plugin KINDS VIEW
    THUMBNAIL assets/preview.png
    HELP help.md)
