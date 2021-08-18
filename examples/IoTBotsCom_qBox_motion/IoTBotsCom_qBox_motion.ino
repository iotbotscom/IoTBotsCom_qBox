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


    Filename: IoTBotsCom_qBox_motion.ino

    General Description: qBody Boards Motion Sensor Example (I2C, LIS3DH)

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
BOOL motion_interrupt = FALSE;

/*---- Function prototypes -------------------------------------------------------------*/

/*---- Function declarations------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
void setup()
{
    LOGGING_SETTINGS_T logging_mode;
    //LIS3DH_SETTINGS_T lm75_settings;

    // SCI Init and On Begin
    SCI_Init();

    SCI_On();
    // SCI Init and On End

    LOG_SYS("\r\n***************************************************************************************************");
    LOG_SYS(" **** IOT-BOTS.COM **** ");
    LOG_SYS(" **** The qBox Motion Example **** ");
    LOG_SYS("***************************************************************************************************\r\n");

    // Log Man Setup Begin
    Logman_Get_Settings(&logging_mode);

    // Enable User Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_ENABLE);
    // Enable User Device Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_DEVICE);
    // Enable User Motion Sensor Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_MSENSOR);

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

    // Motion sensor Begin
    LIS3DH_Init();

    if(LIS3DH_On() != TRUE)
    {
        LOG_DEV("LIS3DH is not enabled or detected\r\n");
    }
    else
    {
        LOG_DEV("LIS3DH is On\r\n");

        //LIS3DH_Config(&lm75_settings);
    }
    // Motion sensor End

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
    LIS3DH_SAMPLE_T msensor;
    LIS3DH_INT_SOURCE_T int_source;

    if(LIS3DH_Get_Data(&msensor) == TRUE)
    {
        digitalWrite(GREEN_LED_PIN, HIGH);
        delay(100);
        digitalWrite(GREEN_LED_PIN, LOW);

        if(LOG_USR_CHECK(USR_LOG_DEVICE))
        {
            Serial.print("LIS3DH :");
            Serial.print(" x = ");
            Serial.print(msensor.x);
            Serial.print(" y = ");
            Serial.print(msensor.y);
            Serial.print(" z = ");
            Serial.print(msensor.z);
            Serial.println("");
        }
    }
    else
    {
        delay(100);

        LOG_DEV("LIS3DH is not ready\r\n");
    }

    LIS3DH_Get_IntSource(&int_source);
    if(int_source.active == TRUE)
    {
        LOG_DEV("LIS3DH : Active Interrupt\r\n");
    }

    if(int_source.motion == TRUE)
    {
        LOG_DEV("LIS3DH : Motion Interrupt\r\n");
    }

    if(int_source.shoke == TRUE)
    {
        LOG_DEV("LIS3DH : Shoke Interrupt\r\n");
    }

    if(motion_interrupt == TRUE)
    {
        motion_interrupt = FALSE;

        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);

        LOG_DEV("LIS3DH : Event Motion\r\n");
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

