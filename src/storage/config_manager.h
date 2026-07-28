#pragma once

#include <Preferences.h>
#include "../common/types.h"

class ConfigManager
{
public:

    void begin();

    bool load();

    void save();

    SystemConfig config;

private:

    Preferences prefs;
};
