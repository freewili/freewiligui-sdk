#ifndef FREEWILIGUI_FREEWILI_H
#define FREEWILIGUI_FREEWILI_H
#include <freewiligui/types.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct FwGuiFreeWiliApi {
    uint32_t struct_size;
    FwGuiStrView (*list_devices)(FwGuiHost* host);
    FwGuiStrView (*list_commands)(FwGuiHost* host);
    FwGuiResult (*submit)(FwGuiHost* host, FwGuiPlugin* plugin,
                          const char* device, const char* command,
                          const char* arguments_json, uint64_t* job);
    FwGuiStrView (*job_result)(FwGuiHost* host, FwGuiPlugin* plugin, uint64_t job);
    FwGuiResult (*cancel_job)(FwGuiHost* host, FwGuiPlugin* plugin, uint64_t job);
    FwGuiResult (*release_job)(FwGuiHost* host, FwGuiPlugin* plugin, uint64_t job);
    FwGuiResult (*subscribe_events)(FwGuiHost* host, FwGuiPlugin* plugin,
                                    const char* device, const char* event_filter,
                                    uint64_t* subscription);
    FwGuiStrView (*read_events)(FwGuiHost* host, FwGuiPlugin* plugin,
                                uint64_t subscription, uint32_t max_events);
    FwGuiResult (*unsubscribe_events)(FwGuiHost* host, FwGuiPlugin* plugin,
                                      uint64_t subscription);
} FwGuiFreeWiliApi;
#ifdef __cplusplus
}
#endif
#endif
