#include "ttgo_power.h"
#include "../common/pins.h"

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

#define I2C_SDA_2 18
#define I2C_SCL_2 19

TwoWire I2CPower(1);
TwoWire I2Cbuttons(0);
bool setPowerBoostKeepOn(uint8_t on)
{
    I2CPower.beginTransmission(IP5306_ADDR);

    I2CPower.write(IP5306_REG_SYS_CTL0);

    I2CPower.write(
        on ? 0x37 : 0x35);

    return
        I2CPower.endTransmission()
        == 0;
}
void initTTGOPower()
{
    I2CPower.begin(
        I2C_SDA,
        I2C_SCL,
        400000U);
	I2Cbuttons.setTimeOut(20);
    I2Cbuttons.begin(
        I2C_SDA_2,
        I2C_SCL_2,
        400000U);

    setPowerBoostKeepOn(1);

    pinMode(
        MODEM_PWKEY,
        OUTPUT);

    pinMode(
        MODEM_RST,
        OUTPUT);

    pinMode(
        MODEM_POWER_ON,
        OUTPUT);

    digitalWrite(
        MODEM_PWKEY,
        LOW);

    digitalWrite(
        MODEM_RST,
        HIGH);

    digitalWrite(
        MODEM_POWER_ON,
        HIGH);
}