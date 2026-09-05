cmake_minimum_required(VERSION 3.21)
project(freewili_plugin LANGUAGES CXX)
find_package(freewiligui 0.1 CONFIG REQUIRED)
freewiligui_add_plugin(my_plugin
    ID org.example.my-plugin
    SYMBOL my_plugin
    SOURCES src/plugin.cpp
    PLUGIN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
