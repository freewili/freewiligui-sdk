# FreeWili GUI SDK

FreeWili GUI SDK 0.1.0 is a C99 ABI and C++17 authoring layer for native GUI
plugins. It provides host logging and settings, ImGui/ImPlot integration,
views, embedded help, Canvas drawing, agent tools, and asynchronous FreeWili
device work.

The ABI version is 1. Plugins export fwguiPluginQuery and fwguiPluginCreate.
All calls and callbacks run on the host owner thread. Borrowed strings remain
valid only for the current host callback or frame.

See [Quick start](docs/QUICKSTART.md), [API and ownership](docs/API.md),
[Canvas](docs/CANVAS.md), [agent tools](docs/MCP.md), and
[FreeWili devices](docs/FREEWILI.md).

The current license is intentionally restrictive while the project owner
selects a redistribution license.
