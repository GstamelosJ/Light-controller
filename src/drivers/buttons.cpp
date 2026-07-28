#include "buttons.h"

#include <Bounce2.h>

#include "../common/pins.h"
#include "../common/globals.h"
static Bounce btnOK;
static Bounce btnUP;
static Bounce btnDOWN;
static Bounce btnESC;
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

 

    if(btnOK.fell())

    {

        Serial.println("OK FELL");

        Menu.buttonOK();

    }

 

    if(btnOK.rose())

    {

        Serial.println("OK ROSE");

    }

 

    if(btnUP.fell())

    {

        Serial.println("UP FELL");

        Menu.buttonUp();

    }

 

    if(btnUP.rose())

    {

        Serial.println("UP ROSE");

    }

 

    if(btnDOWN.fell())

    {

        Serial.println("DOWN FELL");

        Menu.buttonDown();

    }

 

    if(btnDOWN.rose())

    {

        Serial.println("DOWN ROSE");

    }

 

    if(btnESC.fell())

    {

        Serial.println("ESC FELL");

        Menu.buttonESC();

    }

 

    if(btnESC.rose())

    {

        Serial.println("ESC ROSE");

    }

}
