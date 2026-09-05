#include <freewiligui/plugin.hpp>
#include <type_traits>

static_assert(std::is_standard_layout<FwGuiPluginDesc>::value, "public ABI must be standard-layout");
static_assert(std::is_standard_layout<FwGuiCanvasApi>::value, "canvas ABI must be standard-layout");
static_assert(!std::is_copy_constructible<fwgui::McpReply>::value,
              "borrowed replies must not be copied");

int main() {
    fwgui::Host host;
    return host.HasCanvas() || host.HasMcp() || host.HasFreeWili();
}
