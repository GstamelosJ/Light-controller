#include "relay_manager.h"

void RelayManager::begin(
    ChannelConfig channels[])
{
    _channels = channels;

    for(int i=0;i<MAX_CHANNELS;i++)
    {
        pinMode(
            _channels[i].relayPin,
            OUTPUT);

        digitalWrite(
            _channels[i].relayPin,
            LOW);
    }
}

void RelayManager::set(
    uint8_t channel,
    bool state)
{
    if(channel >= MAX_CHANNELS)
        return;

    digitalWrite(
        _channels[channel].relayPin,
        state);

    _channels[channel].currentState =
        state;
}

bool RelayManager::get(
    uint8_t channel)
{
    return _channels[channel]
            .currentState;
}
