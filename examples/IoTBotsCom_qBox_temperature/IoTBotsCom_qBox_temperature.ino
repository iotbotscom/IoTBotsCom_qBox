/*========================================================================================
Copyright (c) 2021 IOT-BOTS.COM LLC. All right reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USEOR OTHER
DEALINGS IN THE SOFTWARE.


    Filename: IoTBotsCom_qBox_temperature.ino

    General Description: qBody Boards Temperature Sensor Example (I2C, LM75)

------------------------------------------------------------------------------------------
Revision History:
                    Modification     Tracking
Author                  Date          Number           Description of Changes
----------------    ------------    ----------   -----------------------------------------
iotbotscom           08/16/2021                  QWARKS FrameWork Demo Initial Public Release

========================================================================================*/

/*---- Include files -------------------------------------------------------------------*/
#include "iotbotscom.h"

/*---- Defines  ------------------------------------------------------------------------*/

/*---- Typedefs ------------------------------------------------------------------------*/

/*---- Variables -----------------------------------------------------------------------*/

/*---- Function prototypes -------------------------------------------------------------*/

/*---- Function declarations------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
void setup()
{
    LOGGING_SETTINGS_T logging_mode;
    //LM75_SETTINGS_T lm75_settings;

    // SCI Init and On Begin
    SCI_Init();

    SCI_On();
    // SCI Init and On End

    LOG_SYS("\r\n***************************************************************************************************");
    LOG_SYS(" **** IOT-BOTS.COM **** ");
    LOG_SYS(" **** The qBox Temperature Example **** ");
    LOG_SYS("***************************************************************************************************\r\n");

    // Log Man Setup Begin
    Logman_Get_Settings(&logging_mode);

    // Enable User Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_ENABLE);
    // Enable User Device Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_DEVICE);
    // Enable User Temperature Sensor Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_TSENSOR);

    // Enable DBG Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_ENABLE);
    // Enable DBG Temperature Sensor Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_I2C);

    Logman_Set_Settings(&logging_mode);
    // Log Man Setup End

#if ((defined ARDUINO_QBOARDA_ESP32) || (defined ARDUINO_QBOARDB_ESP32))
    // Load Enable Begin
    pinMode(LOAD_ENABLE_PIN, OUTPUT);
    digitalWrite(LOAD_ENABLE_PIN, HIGH);
    // Load Enable End
#endif

    // Temperature sensor Begin
    LM75_Init();

    //lm75_settings.control.is_enabled = TRUE;
    //LM75_Config(&lm75_settings);
    // Temperature sensor End

    // Indication Begin
    pinMode(RED_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);
    pinMode(GREEN_LED_PIN, OUTPUT);
    digitalWrite(GREEN_LED_PIN, LOW);
    pinMode(BLUE_LED_PIN, OUTPUT);
    digitalWrite(BLUE_LED_PIN, LOW);
    // Indication End
}

/*--------------------------------------------------------------------------------------*/
void loop()
{
    LM75_SAMPLE_T tsensor;

    if(LM75_On() == TRUE)
    {
        digitalWrite(RED_LED_PIN, HIGH);

        LOG_DEV("LM75 is On\r\n");

        delay(1000);

        if(LM75_Get_Data(&tsensor) == TRUE)
        {
            digitalWrite(GREEN_LED_PIN, HIGH);
            delay(100);
            digitalWrite(GREEN_LED_PIN, LOW);

            if(LOG_USR_CHECK(USR_LOG_DEVICE))
            {
                Serial.println(String("LM75 : ") + (((float)tsensor.temperature) / 2.0) + String(" C"));
            }
        }
        else
        {
            delay(100);

            LOG_DEV("LM75 is not ready\r\n");
        }

        if(LM75_Off() == TRUE)
        {
            LOG_DEV("LM75 is Off\r\n");

            digitalWrite(RED_LED_PIN, LOW);
        }
        else
        {
            LOG_DEV("LM75 Off Failed\r\n");
        }
    }
    else
    {
        LOG_DEV("LM75 is not enabled or detected\r\n");
    }

    SCI_Processor();

    LOG_SYS("");

    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLUE_LED_PIN, LOW);

    delay(800);
}
