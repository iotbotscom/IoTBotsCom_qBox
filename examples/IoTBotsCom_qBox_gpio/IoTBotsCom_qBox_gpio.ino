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


    Filename: IoTBotsCom_qBox_gpio.ino

    General Description: qBody Boards GPIO Example

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
int out0 =  0;
int out1 =  0;

/*---- Function prototypes -------------------------------------------------------------*/

/*---- Function declarations------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
void setup()
{
    LOGGING_SETTINGS_T logging_mode;

    // SCI Init and On Begin
    SCI_Init();

    SCI_On();
    // SCI Init and On End

    LOG_SYS("\r\n***************************************************************************************************");
    LOG_SYS(" **** IOT-BOTS.COM **** ");
    LOG_SYS(" **** The qBox GPIO Example **** ");
    LOG_SYS("***************************************************************************************************\r\n");

    // Log Man Setup Begin
    Logman_Get_Settings(&logging_mode);

    // Enable User Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_ENABLE);
    // Enable User Device Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_DEVICE);
    // Enable User GPIO Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_GPIO);

    Logman_Set_Settings(&logging_mode);
    // Log Man Setup Begin End

    // ADC Resolution
    analogReadResolution(12);

    // GPIO Begin
#if ((defined ARDUINO_QBOARDA_ESP32) || (defined ARDUINO_QBOARDB_ESP32))
    pinMode(LOAD_ENABLE_PIN, OUTPUT);
    digitalWrite(LOAD_ENABLE_PIN, LOW);
#endif

    pinMode(OUT0_PIN, OUTPUT);
    digitalWrite(OUT0_PIN, LOW);

    pinMode(OUT1_PIN, OUTPUT);
    digitalWrite(OUT1_PIN, LOW);
    // GPIO End

    // LEDs Begin
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    // LEDs End
}

/*--------------------------------------------------------------------------------------*/
void loop()
{
    int hyst = 1000;
    float VBat = 0;
    float VExt = 0;
    float VIN0 = 0;
    float VIN1 = 0;

    // Bat voltage
#if (((defined PRODUCT_QWARK_AFC) && (PRODUCT_QWARK_AFC == TRUE)) || ((defined PRODUCT_QWARK_AMC) && (PRODUCT_QWARK_AMC == TRUE)))
    digitalWrite(LOAD_ENABLE_PIN, HIGH);
    delay(500);
#endif
    VBat = (float)analogRead(VINT_ADC_PIN);
#if (((defined PRODUCT_QWARK_AFC) && (PRODUCT_QWARK_AFC == TRUE)) || ((defined PRODUCT_QWARK_AMC) && (PRODUCT_QWARK_AMC == TRUE)))
    digitalWrite(LOAD_ENABLE_PIN, LOW);
#endif
    if(LOG_DBG_CHECK(DBG_LOG_GPIO))
    {
        Serial.print("VBat ADC: ");
        Serial.println(VBat);
    }
    VBat = (250.0 / 150.0) * (3300.0 * VBat) / 4096.0;

    // Ext voltage
    VExt = (float)analogRead(VEXT_ADC_PIN);
    if(LOG_DBG_CHECK(DBG_LOG_GPIO))
    {
        //Serial.printf("VExt ADC : %04.0f\r\n", VExt);
        Serial.print("VExt ADC: ");
        Serial.println(VExt);
    }
    VExt = (110.0 / 10.0) * (3300.0 * VExt) / 4096.0;

    // IN0 voltage
    VIN0 = (float)analogRead(IN0_ADC_PIN);
    if(LOG_DBG_CHECK(DBG_LOG_GPIO))
    {
        //Serial.printf(" IN0 ADC : %04.0f\r\n", VIN0);
        Serial.print(" IN0 ADC: ");
        Serial.println(VIN0);
    }
    VIN0 = (110.0 / 10.0) * (3300.0 * VIN0) / 4096.0;

    // IN1 voltage
    VIN1 = (float)analogRead(IN1_ADC_PIN);
    if(LOG_DBG_CHECK(DBG_LOG_GPIO))
    {
        //Serial.printf(" IN1 ADC : %04.0f\r\n", VIN1);
        Serial.print(" IN1 ADC: ");
        Serial.println(VIN1);
    }
    VIN1 = (110.0 / 10.0) * (3300.0 * VIN1) / 4096.0;

    if(VIN0 > (10000 + hyst))
    {
        digitalWrite(OUT0_PIN, LOW);
        digitalWrite(OUT1_PIN, HIGH);
        out0 = 0;
        out1 = 1;

        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(BLUE_LED_PIN, HIGH);
    }
    else if(VIN0 < (10000 - hyst))
    {
        digitalWrite(OUT1_PIN, LOW);
        digitalWrite(OUT0_PIN, HIGH);
        out0 = 1;
        out1 = 0;

        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(BLUE_LED_PIN, LOW);
    }
    else
    {
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(BLUE_LED_PIN, LOW);
    }

    if(LOG_USR_CHECK(USR_LOG_GPIO))
    {
        //Serial.printf("VExt : %.2f V, IN0 : %.2f V, IN1 : %.2f V, OUT0 : %d, OUT1 : %d", VExt / 1000.0, VIN0 / 1000.0, VIN1 / 1000.0, out0, out1);
        Serial.print("VBat : ");
        Serial.print(VBat / 1000.0);
        Serial.print(", VExt : ");
        Serial.print(VExt / 1000.0);
        Serial.print(", IN0 : ");
        Serial.print(VIN0 / 1000.0);
        Serial.print(", IN1 : ");
        Serial.print(VIN1 / 1000.0);
        Serial.print(", OUT0 : ");
        Serial.print(out0);
        Serial.print(", OUT1 : ");
        Serial.print(out1);
        Serial.println("");
    }

    SCI_Processor();

    LOG_SYS("");

    // LEDs On
    //digitalWrite(RED_LED_PIN, HIGH);
    //digitalWrite(GREEN_LED_PIN, HIGH);
    //digitalWrite(BLUE_LED_PIN, HIGH);

    delay(100);

    // LEDs Off
    //digitalWrite(RED_LED_PIN, LOW);
    //digitalWrite(GREEN_LED_PIN, LOW);
    //digitalWrite(BLUE_LED_PIN, LOW);

    delay(900);
}

