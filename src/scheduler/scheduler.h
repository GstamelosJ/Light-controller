#pragma once

#include "../common/types.h"
#include "../outputs/relay_manager.h"
#include "../astro/astro_manager.h"

class Scheduler
{
public:

    void begin(
        ChannelConfig channels[],
        RelayManager* relays,
        AstroManager* astro);

    void run();

private:

    ChannelConfig* _channels;

    RelayManager* _relays;

    AstroManager* _astro;

    int _lastMinute = -1;
};