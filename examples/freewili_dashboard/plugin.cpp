#include <freewiligui/plugin.hpp>
#include <nlohmann/json.hpp>
#include <string>

using Json = nlohmann::json;

class DeviceDashboard : public fwgui::Plugin<DeviceDashboard> {
public:
    FwGuiResult OnStartup(fwgui::Host) {
        RegisterDoc("index", "FreeWili Dashboard",
            "# FreeWili Dashboard\nSelect a device and command, edit the JSON arguments, "
            "and submit an asynchronous job. Cancel may not stop work already sent.");
        RegisterTool<&DeviceDashboard::Status>(
            "dashboard_status", "Read the dashboard selection and current job",
            "{\"type\":\"object\",\"properties\":{}}");
        fwgui::View view;
        view.id = "dashboard";
        view.title = "FreeWili Devices";
        view.menu_path = "Tools/FreeWili Devices";
        return RegisterView<&DeviceDashboard::Draw>(view);
    }
    void Draw(const fwgui::ViewCtx& ctx) {
        auto host = ctx.host();
        if (!host.HasFreeWili()) {
            ImGui::TextWrapped("This host does not provide the FreeWili device service.");
            return;
        }
        if (devices_.is_null() || ImGui::Button("Refresh devices")) {
            devices_ = Parse(host.FreeWiliListDevices());
            commands_ = Parse(host.FreeWiliListCommands());
        }
        bool present = false;
        if (devices_.is_array()) {
            for (const auto& device : devices_) {
                std::string id = device.value("id", std::string());
                if (id == selected_) present = true;
                if (ImGui::Selectable((device.value("name", id) + " (" + id + ")").c_str(),
                                      id == selected_))
                    selected_ = id;
            }
        }
        if (commands_.is_array()) {
            for (const auto& command : commands_) {
                std::string name = command.value("name", std::string());
                if (ImGui::Selectable(name.c_str(), name == command_)) command_ = name;
            }
        }
        ImGui::InputTextMultiline("Arguments", arguments_, sizeof(arguments_), ImVec2(-1, 90));
        ImGui::BeginDisabled(!present || running_);
        if (ImGui::Button("Submit")) {
            if (job_) host.FreeWiliReleaseJob(job_);
            job_ = 0;
            FwGuiResult result = host.FreeWiliSubmit(
                selected_.c_str(), command_.c_str(), arguments_, &job_);
            running_ = result == FWGUI_OK;
            message_ = running_ ? "Queued" : "Submit failed";
        }
        ImGui::EndDisabled();
        if (running_) {
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) host.FreeWiliCancelJob(job_);
        }
        if (job_) {
            Json value = Parse(host.FreeWiliJobResult(job_));
            if (value.is_object()) {
                std::string state = value.value("state", std::string());
                running_ = state == "queued" || state == "running";
                message_ = value.dump(2);
            }
        }
        ImGui::TextWrapped("%s", message_.c_str());
    }
    void Status(std::string_view, const fwgui::McpReply& reply) {
        reply.Ok(Json{{"device", selected_}, {"command", command_},
                      {"job", job_}, {"running", running_}}.dump().c_str());
    }
private:
    static Json Parse(std::string_view text) {
        return Json::parse(text.begin(), text.end(), nullptr, false);
    }
    Json devices_;
    Json commands_;
    std::string selected_;
    std::string command_ = "io_gpio_read_all";
    std::string message_;
    char arguments_[8192] = "{}";
    uint64_t job_ = 0;
    bool running_ = false;
};

static const FwGuiPluginDesc descriptor = {
    sizeof(FwGuiPluginDesc), "org.freewili.dashboard", "FreeWili Dashboard",
    "1.0.0", "FreeWili", "Runs device commands through owner-scoped jobs.",
    nullptr, FWGUI_FINGERPRINT_INIT
};
FWGUI_PLUGIN_MAIN(freewili_dashboard, &descriptor, DeviceDashboard)
