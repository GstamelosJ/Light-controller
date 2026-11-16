#pragma once

#include <LiquidCrystal_I2C.h>

class LCDManager
{
public:

    void begin();

    void clear();

    void showStatus();

    void showChannelList();

    void showChannel();

    void showEventList();

    void showEventEditor();
	
	void showDaysEditor();
	void showEventAction();
	
void printLine(
        uint8_t row,
        const char* text);

private:

    LiquidCrystal_I2C lcd {0x27,20,4};
};