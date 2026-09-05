function(freewiligui_enable_test_assertions target)
    target_compile_options(${target} PRIVATE
        "$<$<COMPILE_LANG_AND_ID:C,MSVC>:/UNDEBUG>"
        "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/UNDEBUG>"
        "$<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-UNDEBUG>"
        "$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-UNDEBUG>")
endfunction()

add_executable(freewiligui_c99 test_headers.c)
target_link_libraries(freewiligui_c99 PRIVATE freewiligui::sdk)
set_property(TARGET freewiligui_c99 PROPERTY C_STANDARD 99)
freewiligui_enable_test_assertions(freewiligui_c99)
add_test(NAME c99_headers COMMAND freewiligui_c99)

add_executable(freewiligui_cxx17_headers test_headers.cpp gui_stubs.cpp)
target_link_libraries(freewiligui_cxx17_headers PRIVATE freewiligui::sdk)
target_include_directories(freewiligui_cxx17_headers PRIVATE mock)
set_property(TARGET freewiligui_cxx17_headers PROPERTY CXX_STANDARD 17)
freewiligui_enable_test_assertions(freewiligui_cxx17_headers)
add_test(NAME cxx17_headers COMMAND freewiligui_cxx17_headers)

add_executable(freewiligui_cxx17_wrappers test_wrappers.cpp gui_stubs.cpp)
target_link_libraries(freewiligui_cxx17_wrappers PRIVATE freewiligui::sdk)
target_include_directories(freewiligui_cxx17_wrappers PRIVATE mock)
set_property(TARGET freewiligui_cxx17_wrappers PROPERTY CXX_STANDARD 17)
freewiligui_enable_test_assertions(freewiligui_cxx17_wrappers)
add_test(NAME cxx17_wrappers COMMAND freewiligui_cxx17_wrappers)

add_executable(freewiligui_callback_guards test_callback_guards.cpp gui_stubs.cpp)
target_link_libraries(freewiligui_callback_guards PRIVATE freewiligui::sdk)
target_include_directories(freewiligui_callback_guards PRIVATE mock)
set_property(TARGET freewiligui_callback_guards PROPERTY CXX_STANDARD 17)
freewiligui_enable_test_assertions(freewiligui_callback_guards)
add_test(NAME callback_guards COMMAND freewiligui_callback_guards)

add_library(freewiligui_shared_c MODULE shared_plugin.c)
target_link_libraries(freewiligui_shared_c PRIVATE freewiligui::sdk)
target_compile_definitions(freewiligui_shared_c PRIVATE FWGUI_PLUGIN_SHARED)
set_target_properties(freewiligui_shared_c PROPERTIES PREFIX "")
if(MSVC)
    get_filename_component(_freewiligui_compiler_dir "${CMAKE_C_COMPILER}" DIRECTORY)
    find_program(_freewiligui_symbol_inspector dumpbin
        HINTS "${_freewiligui_compiler_dir}" REQUIRED)
    set(_freewiligui_symbol_style MSVC)
else()
    set(_freewiligui_symbol_inspector "${CMAKE_NM}")
    set(_freewiligui_symbol_style GNU)
endif()
add_test(NAME shared_plugin_exports
    COMMAND "${CMAKE_COMMAND}"
        "-DMODULE=$<TARGET_FILE:freewiligui_shared_c>"
        "-DINSPECTOR=${_freewiligui_symbol_inspector}"
        "-DSTYLE=${_freewiligui_symbol_style}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_exports.cmake")

if(FREEWILIGUI_PRIVACY_TERMS_FILE)
    add_test(NAME source_privacy
        COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DTERMS_FILE=${FREEWILIGUI_PRIVACY_TERMS_FILE}"
            -P "${PROJECT_SOURCE_DIR}/tools/audit_source.cmake")
endif()
