#include "remote_buttons.h"

#include <Arduino.h>

#include "../common/globals.h"
#include "../common/types.h"
#include "../platform/ttgo_power.h"

#define REMOTE_SLAVE_ADDR 0x08

void RemoteButtons::probeDevice()
{
    I2Cbuttons.beginTransmission(
        REMOTE_SLAVE_ADDR);

    _available =
        (I2Cbuttons.endTransmission() == 0);

    if(_available)
    {
        Serial.println(
            "[REMOTE] Slave detected");
    }
    else
    {
        Serial.println(
            "[REMOTE] Slave not found");
    }
}

void RemoteButtons::begin()
{
    probeDevice();

    _lastProbe = millis();
}

bool RemoteButtons::isAvailable() const
{
    return _available;
}

void RemoteButtons::update()
{
    /*
     * Αν δεν βρέθηκε slave,
     * επαναλαμβάνουμε probe κάθε 10 sec
     */
    if(!_available)
    {
        if(millis() - _lastProbe > 10000)
        {
            _lastProbe = millis();

            probeDevice();
        }

        return;
    }

    if(I2Cbuttons.requestFrom(
            (uint8_t)REMOTE_SLAVE_ADDR,
            (uint8_t)1) != 1)
    {
        return;
    }

    uint8_t button =
        I2Cbuttons.read();

    if(button == 0xFF)
    {
        return;
    }

    if(button == 0x00)
    {
        return;
    }

    uint8_t channel =
        button - 1;

    if(channel >= MAX_CHANNELS)
    {
        return;
    }

    bool newState =
        !Config.config
              .channels[channel]
              .currentState;

    Relays.set(
        channel,
        newState);
}