cmake_minimum_required(VERSION 3.21)
project(freewili_panel_control LANGUAGES CXX)
find_package(freewiligui 0.2 CONFIG REQUIRED)
freewiligui_add_plugin(my_control NO_IMGUI
    ID com.example.my-control
    SYMBOL my_control
    SOURCES src/control.cpp
    PLUGIN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
