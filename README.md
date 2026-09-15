# FreeWili GUI SDK

Build native plugins for [FreeWili GUI](https://github.com/freewili/freewili-gui)
with a standalone C99 ABI and C++17 authoring layer. SDK version **0.1.0**
uses **ABI version 1**.

## Features

- Dockable ImGui/ImPlot views and native Canvas drawing.
- Host logging, persistent settings, and embedded Markdown help.
- Agent tools with JSON Schema inputs.
- Asynchronous FreeWili device commands, jobs, and event subscriptions.
- An installable CMake package, a minimal plugin template, and two examples.

## Get started

You need Git, CMake 3.21 or newer, and C99/C++17 compilers. The SDK and its
contract tests build independently of the application. Building plugins also
fetches Dear ImGui and ImPlot from their public repositories unless you supply
local dependencies.

```sh
git clone https://github.com/freewili/freewiligui-sdk.git
cd freewiligui-sdk
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix "$PWD/sdk-install"
cmake -S plugin-template -B plugin-build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PWD/sdk-install"
cmake --build plugin-build --config Release
```

These commands work in PowerShell and POSIX shells. Match your plugin's
architecture, compiler/runtime, build configuration, and GUI fingerprint to
the application before loading it. See the [quick start](docs/QUICKSTART.md)
for Windows configuration, dependency options, example builds, and installation.

## Plugin contract

The ABI version is 1. Plugins export fwguiPluginQuery and fwguiPluginCreate.
All calls and callbacks run on the host owner thread. Borrowed strings remain
valid only for the current host callback or frame.

See [API and ownership](docs/API.md),
[Canvas](docs/CANVAS.md), [agent tools](docs/MCP.md), and
[FreeWili devices](docs/FREEWILI.md).

## License

[MIT](LICENSE). See [third-party notices](THIRD_PARTY_NOTICES.md) for optional
dependencies and their licenses.
