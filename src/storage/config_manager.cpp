#include "config_manager.h"

void ConfigManager::begin()
{
    prefs.begin("lighting", false);
}

bool ConfigManager::load()
{
    size_t len =
        prefs.getBytesLength("system");

    if(len != sizeof(SystemConfig))
        return false;

    prefs.getBytes(
        "system",
        &config,
        sizeof(SystemConfig));

    return true;
}

void ConfigManager::save()
{
    prefs.putBytes(
        "system",
        &config,
        sizeof(SystemConfig));
}
