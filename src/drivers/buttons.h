#pragma once

#include <stdint.h>

class Buttons
{
public:

    void begin();

    void update();

private:
    uint32_t _wifiComboStartedAt = 0;
    bool _wifiComboTriggered = false;

    void updateWiFiResetCombo();
};