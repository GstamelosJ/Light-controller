#include "buttons.h"

#include <Bounce2.h>

#include "../common/pins.h"
#include "../common/globals.h"
static Bounce btnOK;
static Bounce btnUP;
static Bounce btnDOWN;
static Bounce btnESC;
static uint32_t upPressedTime = 0;
static uint32_t downPressedTime = 0;

static uint32_t upRepeatTime = 0;
static uint32_t downRepeatTime = 0;

void Buttons::begin()
{
    pinMode(BTN_OK, INPUT);

    pinMode(BTN_UP, INPUT);

    pinMode(BTN_DOWN, INPUT);

    pinMode(BTN_ESC, INPUT);

    //btnOK.setPressedState(LOW);
    btnOK.attach(BTN_OK);
   // btnUP.setPressedState(LOW);
    btnUP.attach(BTN_UP);
    //btnDOWN.setPressedState(LOW);
    btnDOWN.attach(BTN_DOWN);
    //btnESC.setPressedState(LOW);
    btnESC.attach(BTN_ESC);

    btnOK.interval(10);
    btnUP.interval(10);
    btnDOWN.interval(10);
    btnESC.interval(10);
}
void Buttons::update()
{
    btnOK.update();
    btnUP.update();
    btnDOWN.update();
    btnESC.update();
	updateWiFiResetCombo();

	if(_wifiComboStartedAt != 0)
	{
		/*
		 * Suppress normal OK and ESC actions
		 * while the combination is held.
		 */
		return;
	}
    /*
     * OK
     */

    if(btnOK.fell())
    {
        Menu.buttonOK();
    }

    /*
     * ESC
     */

    if(btnESC.fell())
    {
        Menu.buttonESC();
    }

    /*
     * UP
     */

    if(btnUP.fell())
    {
        Menu.buttonUp();

        upPressedTime =
            millis();

        upRepeatTime =
            millis();
    }

    if(btnUP.read() == LOW)
    {
        uint32_t now =
            millis();

        uint32_t repeatPeriod =
            0;

        if(now - upPressedTime > 3000)
        {
            repeatPeriod = 100;      // 10/sec
        }
        else if(now - upPressedTime > 500)
        {
            repeatPeriod = 500;      // 2/sec
        }

        if(repeatPeriod &&
           now - upRepeatTime >= repeatPeriod)
        {
            upRepeatTime = now;

            Menu.buttonUp();
        }
    }

    if(btnUP.rose())
    {
        upPressedTime = 0;
    }

    /*
     * DOWN
     */

    if(btnDOWN.fell())
    {
        Menu.buttonDown();

        downPressedTime =
            millis();

        downRepeatTime =
            millis();
    }

    if(btnDOWN.read() == LOW)
    {
        uint32_t now =
            millis();

        uint32_t repeatPeriod =
            0;

        if(now - downPressedTime > 3000)
        {
            repeatPeriod = 100;      // 10/sec
        }
        else if(now - downPressedTime > 500)
        {
            repeatPeriod = 500;      // 2/sec
        }

        if(repeatPeriod &&
           now - downRepeatTime >= repeatPeriod)
        {
            downRepeatTime = now;

            Menu.buttonDown();
        }
    }

    if(btnDOWN.rose())
    {
        downPressedTime = 0;
    }
}
void Buttons::updateWiFiResetCombo()
{
    bool okPressed =
        digitalRead(BTN_OK) == LOW;

    bool escPressed =
        digitalRead(BTN_ESC) == LOW;

    if(okPressed &&
       escPressed)
    {
        if(_wifiComboStartedAt == 0)
        {
            _wifiComboStartedAt =
                millis();

            _wifiComboTriggered =
                false;
        }

        if(!_wifiComboTriggered &&
           millis() -
               _wifiComboStartedAt >=
               5000)
        {
            _wifiComboTriggered =
                true;

            Serial.println(
                "[WIFI] Provisioning requested "
                "by OK+ESC.");

            LCD.clear();

            WifiMgr
                .resetCredentialsAndStartProvisioning();
        }

        return;
    }

    _wifiComboStartedAt = 0;
    _wifiComboTriggered = false;
}