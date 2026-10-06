# API and ownership

The public C ABI is in include/freewiligui. Every extensible table begins with
struct_size. Before using an appended field, call FWGUI_HAS on the table and
field. A host may omit an entire optional family or provide a shorter table.

FwGuiHostApi contains only the ABI version and the Core, UI, Canvas, agent-tool,
and FreeWili families. A plugin receives the root table during startup and may
retain it until shutdown returns. The host owns its tables and opaque host
cookie. The plugin owns its instance; its shutdown callback must destroy that
instance. A failed startup must clean up immediately.

All host calls and plugin callbacks use the host owner thread. FwGuiStrView
points to borrowed UTF-8 bytes and may not be NUL terminated. Copy any value
needed after the current callback or frame. The host copies view, document,
tool, and plugin registration strings.

Views are registered during startup. The host normally owns Begin/End and view
visibility. FWGUI_VIEW_OWNS_WINDOW transfers window drawing to the plugin.
FWGUI_VIEW_MENUBAR requests a menu bar on a host-managed window.

Settings, jobs, event subscriptions, views, documents, and tools are scoped to
the plugin instance that created them. A plugin must never pass another
instance's opaque values.

## Limits

- Call host functions only on the host owner thread, except a function whose
  documentation explicitly permits another thread.
- Do not retain FwGuiStrView data after the current callback or frame.
- Do not dereference opaque host, plugin, Canvas, image, reply, job, or
  subscription handles.
- Do not let exceptions cross the C ABI boundary.
- Treat absent families, shorter tables, and null optional callbacks as normal
  compatibility states.
- Match the host's Dear ImGui, ImPlot, runtime-library, and test-engine build
  fingerprint.

For release audits, set FREEWILIGUI_PRIVACY_TERMS_FILE to an external,
newline-delimited deny-list. The test suite scans source paths and contents
for the supplied terms.

Panel controls use the FwGuiPanelControlApi table, reached through the root table's panel_control field. Check FWGUI_HAS(api, panel_control) and a non-null pointer before use. See [panel controls](PANEL_CONTROLS.md) for its callbacks and rules.
