#pragma once

#include "../common/types.h"

class RelayManager
{
public:

    void begin(
        ChannelConfig channels[]);

    void set(
        uint8_t channel,
        bool state);

    bool get(
        uint8_t channel);

private:

    ChannelConfig* _channels;
};