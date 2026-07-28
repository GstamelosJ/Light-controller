#pragma once

#include <cstdint>

class RemoteButtons
{
public:

    void begin();

    void update();

    bool isAvailable() const;

private:

    bool _available = false;

    uint32_t _lastProbe = 0;

    void probeDevice();
};