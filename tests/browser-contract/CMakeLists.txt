cmake_minimum_required(VERSION 3.21)
project(freewiligui_browser_contract_tests LANGUAGES C CXX)
enable_testing()
get_filename_component(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
include("${SDK_ROOT}/cmake/FreeWiliGuiPlugin.cmake")
foreach(mode IN ITEMS c cpp)
    add_executable(browser_${mode} "test_browser.${mode}")
    target_include_directories(browser_${mode} PRIVATE "${SDK_ROOT}/include")
    freewiligui_browser_assets(browser_${mode} KINDS PANEL_CONTROL
        THUMBNAIL assets/thumbnail.png HELP assets/help.md)
    add_test(NAME browser_${mode} COMMAND browser_${mode})
endforeach()
foreach(kind IN ITEMS valid missing legacy)
    add_library(browser_${kind} MODULE fixture.c)
    target_include_directories(browser_${kind} PRIVATE "${SDK_ROOT}/include")
    freewiligui_browser_assets(browser_${kind} KINDS PANEL_CONTROL
        THUMBNAIL assets/thumbnail.png HELP assets/help.md)
endforeach()
target_compile_definitions(browser_missing PRIVATE OMIT_BROWSER)
target_compile_definitions(browser_legacy PRIVATE LEGACY_BROWSER)

foreach(case IN ITEMS missing kind png help)
    if(case STREQUAL "missing")
        set(expected "requires KINDS")
    elseif(case STREQUAL "kind")
        set(expected "Unknown browser kind")
    elseif(case STREQUAL "png")
        set(expected "must be a PNG")
    else()
        set(expected "must contain Markdown")
    endif()
    add_test(NAME assets_${case} COMMAND ${CMAKE_COMMAND}
        -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/invalid
        -DBUILD=${CMAKE_CURRENT_BINARY_DIR}/invalid-${case}
        -DSDK_ROOT=${SDK_ROOT} -DCASE=${case} "-DEXPECT=${expected}"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/verify_invalid.cmake)
endforeach()
