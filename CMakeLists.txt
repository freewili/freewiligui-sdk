cmake_minimum_required(VERSION 3.21)
project(freewiligui VERSION 0.2.0 LANGUAGES C CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(FREEWILIGUI_BUILD_TESTS "Build SDK contract tests" ON)
option(FREEWILIGUI_BUILD_EXAMPLES "Build SDK examples" OFF)
set(FREEWILIGUI_PRIVACY_TERMS_FILE "" CACHE FILEPATH
    "Optional external newline-delimited source deny-list")
set(FREEWILIGUI_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH
    "FreeWili GUI SDK source or install root")

add_library(freewiligui_sdk INTERFACE)
add_library(freewiligui::sdk ALIAS freewiligui_sdk)
set_target_properties(freewiligui_sdk PROPERTIES EXPORT_NAME sdk)
target_include_directories(freewiligui_sdk INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

include(cmake/FreeWiliGuiPlugin.cmake)

if(FREEWILIGUI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(FREEWILIGUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS freewiligui_sdk EXPORT freewiligui-targets)
install(EXPORT freewiligui-targets
    FILE freewiligui-targets.cmake
    NAMESPACE freewiligui::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/freewiligui)
install(FILES
    cmake/FreeWiliGuiPlugin.cmake
    cmake/imgui_test_engine_stubs.cpp
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/freewiligui)

configure_package_config_file(
    cmake/freewiligui-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/freewiligui-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/freewiligui)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/freewiligui-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/freewiligui-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/freewiligui-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/freewiligui)
