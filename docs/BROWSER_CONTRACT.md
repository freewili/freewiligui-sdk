# Plugin browser contract (SDK 0.3.0, ABI 1)

Every newly built plugin must supply a complete `FwGuiPluginDesc` with:

- Nonblank `id`, `name`, `version`, `author`, and `description`.
- `browser` pointing to an immutable `FwGuiPluginBrowserInfo`.
- `kinds` declaring every supplied View, Panel control, or MCP tool capability.
- Embedded PNG thumbnail bytes and byte count (1–4096 pixels per dimension,
  maximum 16 MiB). Supply a real screenshot or representative plugin artwork.
- Nonblank UTF-8 help Markdown, maximum 1 MiB.

`homepage` remains optional. All descriptor pointers and pointed-to bytes must
remain valid until the library is unloaded. Browser metadata is returned from
`fwguiPluginQuery`; obtaining it must not require `startup`, a device connection,
or hardware commands. A disabled plugin must be fully describable.

The browser uses the declared type, thumbnail, and help before startup. A host
copies the data it needs before unloading. Registered runtime help can add
pages; the embedded help is the required Overview page. Thumbnail images are
presentation data, never callbacks that start a view or access a device.

## CMake helper

```cmake
freewiligui_add_plugin(my_plugin NO_IMGUI
    ID org.example.slider SYMBOL my_slider SOURCES plugin.c)
freewiligui_browser_assets(my_plugin KINDS PANEL_CONTROL
    THUMBNAIL preview.png HELP help.md)
```

The helper checks required arguments, file existence, PNG header/dimensions,
size limits, and nonblank help. It embeds bytes in a target-local header and
tracks both input files as configure dependencies. It works with C and C++,
shared/static plugins, and Windows/macOS/Linux; there are no runtime asset paths
or image-library requirements for the plugin.

In the descriptor translation unit:

```c
#include "freewiligui_browser.h"
static const FwGuiPluginDesc descriptor = {
    sizeof(FwGuiPluginDesc), "org.example.slider", "Slider", "1.0.0",
    "Example", "Adjusts a project value", NULL,
    FWGUI_FINGERPRINT_INIT_NO_IMGUI, FWGUI_PLUGIN_NO_IMGUI,
    FWGUI_BROWSER_INFO
};
```

Use `KINDS VIEW`, `KINDS PANEL_CONTROL`, `KINDS MCP_TOOL`, or list all applicable
kinds. The public `fwguiPluginDescError` / `fwguiBrowserInfoError` validators
return NULL on valid data and a static diagnostic otherwise. They check bounds
and the PNG header; the host's image decoder remains responsible for decoding.

## Compatibility and migration

The `browser` pointer is appended after `flags`; ABI remains 1. A new host reads
it only when `struct_size` includes that field. Existing SDK 0.2.0 binaries with
a shorter descriptor continue to load through the legacy browser fallback.
New/full-size descriptors missing required data are rejected before startup,
with a diagnostic. Old hosts ignore the appended field. Do not truncate a new
descriptor's size to evade this contract; short descriptors are only for genuine
legacy binaries and compatibility tests.

Rebuilding an old plugin against SDK 0.3.0 requires adding the browser field,
thumbnail, kinds, and help. Existing templates must be migrated using the
pattern above. Canvas Gauge and Panel Slider demonstrate complete embedded
metadata in their example source directories. Their committed generated headers
are standalone examples; use the CMake helper for new projects.

## Verification

```sh
cmake -S tests/browser-contract -B build/browser-contract
cmake --build build/browser-contract
ctest --test-dir build/browser-contract --output-on-failure
```

The C and C++ tests cover valid metadata, required fields, descriptor sizes,
invalid kinds, absent/oversized/invalid PNG headers, and missing help. Shared
fixtures are also built for a host to verify metadata while disabled, rejection
of incomplete new descriptors, and acceptance of a legacy descriptor prefix.
This change was exercised on macOS; Windows/Linux verification is pending.
