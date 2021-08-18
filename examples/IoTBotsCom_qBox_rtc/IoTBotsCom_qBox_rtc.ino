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


    Filename: IoTBotsCom_qBox_rtc.ino

    General Description: qBody Boards RTC Chip Example (I2C, PCF85063A)

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
// Global Vars
BOOL rtc_interrupt = FALSE;

/*---- Function prototypes -------------------------------------------------------------*/

/*---- Function declarations------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
void setup()
{
    LOGGING_SETTINGS_T logging_mode;
    //RTC_SETTINGS_T lm75_settings;

    // SCI Init and On Begin
    SCI_Init();

    SCI_On();
    // SCI Init and On End

    LOG_SYS("\r\n***************************************************************************************************");
    LOG_SYS(" **** IOT-BOTS.COM **** ");
    LOG_SYS(" **** The qBox RTC Example **** ");
    LOG_SYS("***************************************************************************************************\r\n");

    // Log Man Setup Begin
    Logman_Get_Settings(&logging_mode);

    // Enable User Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_ENABLE);
    // Enable User Device Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_DEVICE);
    // Enable User Motion Sensor Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_RTC);

    // Enable DBG Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_ENABLE);
    // Enable DBG Temperature Sensor Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_I2C);

    Logman_Set_Settings(&logging_mode);
    // Log Man Setup Begin End

#if ((defined ARDUINO_QBOARDA_ESP32) || (defined ARDUINO_QBOARDB_ESP32))
    // Load Enable Begin
    pinMode(LOAD_ENABLE_PIN, OUTPUT);
    digitalWrite(LOAD_ENABLE_PIN, HIGH);

    delay(100);
    // Load Enable End
#endif

    // Temperature sensor Begin
    RTC_Init();

    if(RTC_On() != TRUE)
    {
        LOG_DEV("RTC is not enabled or detected\r\n");
    }
    else
    {
        LOG_DEV("RTC is On\r\n");

        //RTC_Config(&lm75_settings);
    }
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
    TD_T td;

    if(RTC_GetTD(&td) == TRUE)
    {
        digitalWrite(GREEN_LED_PIN, HIGH);
        delay(100);
        digitalWrite(GREEN_LED_PIN, LOW);

        if(LOG_USR_CHECK(USR_LOG_DEVICE))
        {
            Serial.print("DT : ");
            //Serial.printf("%02d/%02d/%02d", td.date.month, td.date.day, td.date.year);
            Serial.print(td.date.month);
            Serial.print("/");
            Serial.print(td.date.day);
            Serial.print("/");
            Serial.print(td.date.year);

            Serial.print(" | ");
            //Serial.printf("%02d:%02d:%02d", td.time.hour, td.time.minute, td.time.second);
            Serial.print(td.time.hour);
            Serial.print(":");
            Serial.print(td.time.minute);
            Serial.print(":");
            Serial.print(td.time.second);

            Serial.println("");
        }
    }
    else
    {
        delay(100);

        LOG_DEV("RTC is not ready\r\n");
    }

    if(rtc_interrupt == TRUE)
    {
        rtc_interrupt = FALSE;

        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);

        LOG_DEV("RTC : Event RTC Alarm\r\n");
    }
    else
    {
        delay(100);
    }

    SCI_Processor();

    LOG_SYS("");

    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLUE_LED_PIN, LOW);

    delay(700);
}
