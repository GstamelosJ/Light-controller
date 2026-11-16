#include <Arduino.h>
#include "platform/ttgo_power.h"
#include "common/pins.h"
#include "common/globals.h"
#include "common/event_factory.h"
#include "drivers/remote_buttons.h"

void createDefaults()
{
    uint8_t relayPins[MAX_CHANNELS] =
    {
        RELAY_1,
        RELAY_2,
        RELAY_3,
        RELAY_4,
        RELAY_5,
        RELAY_6,
        RELAY_7,
        RELAY_8
    };

    for(int i = 0; i < MAX_CHANNELS; i++)
    {
        Config.config.channels[i].enabled = true;

        Config.config.channels[i].mode =
            AUTO_MODE;

        Config.config.channels[i].currentState =
            false;

        Config.config.channels[i].relayPin =
            relayPins[i];

        Config.config.channels[i].eventCount =
            0;
    }

    //--------------------------------------------------
    // Default Location
    //--------------------------------------------------

    Config.config.location.latitude =
        40.6401;

    Config.config.location.longitude =
        22.9444;

    //--------------------------------------------------
    // Network Defaults
    //--------------------------------------------------

    strcpy(
        Config.config.network.apn,
        "internet");

    strcpy(
        Config.config.network.apnUser,
        "");

    strcpy(
        Config.config.network.apnPass,
        "");

    strcpy(
        Config.config.network.wifiSSID,
        "COSMOTE-189DDC");

    strcpy(
        Config.config.network.wifiPASS,
        "UXYebdfUddddKqAq");

    //--------------------------------------------------
    // Demo Schedule CH1
    //--------------------------------------------------

    Config.config.channels[0].eventCount = 2;

    // Sunset +15 minutes => ON

    Config.config.channels[0].events[0] =
        createSunsetEvent(
            15,
            true);

    // 23:30 => OFF

    Config.config.channels[0].events[1] =
        createFixedEvent(
            23,
            30,
            false);
}

void setup()
{
	initTTGOPower();
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("Lighting Controller");
    Serial.println("Milestone 2.8");
    Serial.println("--------------------------------");
//wifitest
    strcpy(
        Config.config.network.wifiSSID,
        "COSMOTE-189DDC");

    strcpy(
        Config.config.network.wifiPASS,
        "UXYebdfUddddKqAq");

    //--------------------------------------------------
    // Configuration
    //--------------------------------------------------

    Config.begin();

    if(!Config.load())
    {
        Serial.println(
            "No valid configuration found.");

        Serial.println(
            "Creating defaults...");

        createDefaults();

        Config.save();
    }
    else
    {
        if(strlen(
                Config.config.network.wifiSSID)
                == 0)
        {
            Serial.println(
                "Stored WiFi credentials were empty; restoring defaults.");

            strcpy(
                Config.config.network.wifiSSID,
                "COSMOTE-189DDC");

            strcpy(
                Config.config.network.wifiPASS,
                "UXYebdfUddddKqAq");

            Config.save();
        }

        Serial.println(
            "Configuration loaded.");
    }
	//wifi manager
	WifiMgr.begin();
    //--------------------------------------------------
    // Outputs
    //--------------------------------------------------

    Relays.begin(
        Config.config.channels);

    //--------------------------------------------------
    // Time Manager
    //--------------------------------------------------

    TimeMgr.begin();

    //--------------------------------------------------
    // Astronomy Manager
    //--------------------------------------------------

    Astro.begin(
        Config.config.location.latitude,
        Config.config.location.longitude);

    //--------------------------------------------------
    // Scheduler
    //--------------------------------------------------

    Sched.begin(
        Config.config.channels,
        &Relays,
        &Astro);

    //--------------------------------------------------
    // LCD
    //--------------------------------------------------

    LCD.begin();

    //--------------------------------------------------
    // Menu
    //--------------------------------------------------

    Menu.begin();

    //--------------------------------------------------
    // Buttons
    //--------------------------------------------------

    Btns.begin();
	RemoteKeys.begin();

    //--------------------------------------------------
    // Initial Screen
    //--------------------------------------------------

    LCD.showStatus();
	

    Serial.println(
        "Initialization completed.");
}

void loop()
{
    //--------------------------------------------------
    // Buttons
    //--------------------------------------------------

    Btns.update();
	//RemoteKeys.update();
   static uint32_t lastRemotePoll = 0;
    if(millis() - lastRemotePoll > 100)
    {
        lastRemotePoll = millis();
        RemoteKeys.update();
    }
	
	//wifi manager update
	WifiMgr.update();

    //--------------------------------------------------
    // Time
    //--------------------------------------------------

    TimeMgr.update();

    //--------------------------------------------------
    // Sunrise / Sunset
    //--------------------------------------------------

    Astro.calculate();

    //--------------------------------------------------
    // Scheduler
    //--------------------------------------------------

    Sched.run();

    //--------------------------------------------------
    // LCD Refresh
    //--------------------------------------------------

    static uint32_t uiRefresh = 0;

    if(millis() - uiRefresh > 250)
    {
        uiRefresh = millis();

        Menu.update();
    }
}