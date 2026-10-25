set(FREEWILIGUI_JSON_TARGET "" CACHE STRING "Existing nlohmann JSON target")
if(FREEWILIGUI_JSON_TARGET)
    if(NOT TARGET "${FREEWILIGUI_JSON_TARGET}")
        message(FATAL_ERROR "FREEWILIGUI_JSON_TARGET does not name a target")
    endif()
    set(_json_target "${FREEWILIGUI_JSON_TARGET}")
else()
    include(FetchContent)
    FetchContent_Declare(freewiligui_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(freewiligui_json)
    set(_json_target nlohmann_json::nlohmann_json)
endif()
freewiligui_add_plugin(freewili_dashboard
    ID org.freewili.dashboard
    SYMBOL freewili_dashboard
    SOURCES plugin.cpp
    PLUGIN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
if(TARGET freewili_dashboard)
    target_link_libraries(freewili_dashboard PRIVATE "${_json_target}")
    freewiligui_browser_assets(freewili_dashboard KINDS VIEW MCP_TOOL
        THUMBNAIL assets/preview.png HELP help.md)
endif()
