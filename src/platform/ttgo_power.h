
#pragma once

#include <Arduino.h>
#include <Wire.h>

/*
 * Bus 1
 * IP5306 Power Management
 * GPIO21 / GPIO22
 */
extern TwoWire I2CPower;

/*
 * Bus 0
 * LCD + Arduino Button Slave
 * GPIO18 / GPIO19
 */
extern TwoWire I2Cbuttons;

/*
 * IP5306
 */
bool setPowerBoostKeepOn(uint8_t on);

/*
 * TTGO T-Call power initialization
 */
void initTTGOPower();