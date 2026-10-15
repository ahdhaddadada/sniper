#include "plugin.h"
#include "SniperTriggerBot.h"

using namespace plugin;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        SniperTriggerBot::Init();
    }
    return TRUE;
}
