cmake_minimum_required(VERSION 3.21)
project(freewili_panel_control LANGUAGES CXX)
find_package(freewiligui 0.3 CONFIG REQUIRED)
freewiligui_add_plugin(my_control NO_IMGUI
    ID com.example.my-control
    SYMBOL my_control
    SOURCES src/control.cpp
    PLUGIN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
# Required browser metadata: the picture and help page FreeWili GUI shows
# in Setup > Manage plugins. assets/preview.png is a placeholder so the
# first build loads; replace it with a picture of the finished control.
freewiligui_browser_assets(my_control KINDS PANEL_CONTROL
    THUMBNAIL assets/preview.png
    HELP help.md)
