#pragma once

class MenuManager
{
public:

    void begin();

    void update();

    void buttonUp();

    void buttonDown();

    void buttonOK();

    void buttonESC();

private:

    void applyChannelMode(
        uint8_t channel);
};