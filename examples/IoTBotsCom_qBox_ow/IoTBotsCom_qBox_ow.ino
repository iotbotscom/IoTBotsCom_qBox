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


    Filename: IoTBotsCom_qBox_ow.ino

    General Description: qBody Boards OneWire Example (OneWire, DS18S20)
    Based on Paul Stoffregen OneWire Library : https://github.com/PaulStoffregen/OneWire

------------------------------------------------------------------------------------------
Revision History:
                    Modification     Tracking
Author                  Date          Number           Description of Changes
----------------    ------------    ----------   -----------------------------------------
iotbotscom           08/16/2021                  QWARKS FrameWork Demo Initial Public Release

========================================================================================*/

/*---- Include files -------------------------------------------------------------------*/
#include "iotbotscom.h"
#include <OneWire.h>

/*---- Defines  ------------------------------------------------------------------------*/

/*---- Typedefs ------------------------------------------------------------------------*/

/*---- Variables -----------------------------------------------------------------------*/
// Global Vars
BOOL motion_interrupt = FALSE;
UINT8 active_timer = 0;
OneWire  ds(OWD_PIN);

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
    LOG_SYS(" **** The qBox OneWire Example **** ");
    LOG_SYS("***************************************************************************************************\r\n");

    // Log Man Setup Begin
    Logman_Get_Settings(&logging_mode);

    // Enable User Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_ENABLE);
    // Enable User Device Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_DEVICE);
    // Enable User Motion Sensor Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_OW);

    // Enable DBG Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_ENABLE);
    // Enable DBG Temperature Sensor Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_OW);

    Logman_Set_Settings(&logging_mode);
    // Log Man Setup Begin End

    // GPIO Begin

#if ((defined ARDUINO_QBOARDA_ESP32) || (defined ARDUINO_QBOARDB_ESP32))
    // Load Enable Begin
    pinMode(LOAD_ENABLE_PIN, OUTPUT);
    digitalWrite(LOAD_ENABLE_PIN, HIGH);
    // Load Enable End
#endif

    // Indication
    // Indication Begin
    pinMode(RED_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);
    pinMode(GREEN_LED_PIN, OUTPUT);
    digitalWrite(GREEN_LED_PIN, LOW);
    pinMode(BLUE_LED_PIN, OUTPUT);
    digitalWrite(BLUE_LED_PIN, LOW);
    // Indication End
    // GPIO End

    // Motion sensor Begin
    // Motion sensor End
}

/*--------------------------------------------------------------------------------------*/
void loop()
{
    byte i;
    byte present = 0;
    byte data[12];
    byte addr[8];
    byte family;
    float celsius, fahrenheit;

    SCI_Processor();

    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLUE_LED_PIN, LOW);

    delay(700);

    if(!ds.search(addr))
    {
        LOG_OW("No more addresses\r\n\r\n");
        ds.reset_search();
        delay(250);

        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);

        return;
    }

    if(LOG_DBG_CHECK(DBG_LOG_OW))
    {
        Serial.print("R = ");
        for(i = 0; i < 8; i++)
        {
            Serial.print(addr[i], HEX);
            Serial.print(" ");
        }
    }

    if(OneWire::crc8( addr, 7) != addr[7])
    {
        DBG_OW("CRC is not valid!\r\n");

        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);

        return;
    }

    // Device Family Detection
    switch (addr[0])
    {
        case 0x10:
            DBG_OW("Family = DS18S20");
            family = 1;
            break;

        case 0x28:
            DBG_OW("Family = DS18B20");
            family = 0;
            break;

        case 0x22:
            DBG_OW("Famiiy = DS1822");
            family = 0;
            break;

        default:
            DBG_OW("Device is not a DS18x20 family device");
            digitalWrite(RED_LED_PIN, HIGH);
            delay(100);
            digitalWrite(RED_LED_PIN, LOW);
            return;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);

    delay(750);

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);

    if(LOG_DBG_CHECK(DBG_LOG_OW))
    {
        Serial.print("P = ");
        Serial.print(present,HEX);
        Serial.print(" ");
    }
    for(i = 0; i < 9; i++)
    {
        // we need 9 bytes
        data[i] = ds.read();
        if(LOG_DBG_CHECK(DBG_LOG_OW))
        {
            Serial.print(data[i], HEX);
            Serial.print(" ");
        }
    }
    if(LOG_DBG_CHECK(DBG_LOG_OW))
    {
        Serial.print(" CRC = ");
        Serial.print( OneWire::crc8( data, 8), HEX);
        Serial.println();
    }

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if(family)
    {
        raw = raw << 3; // 9 bit resolution default
        if(data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    }
    else
    {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if(cfg == 0x00)
        {
            raw = raw & ~7;  // 9 bit resolution, 93.75 ms
        }
        else if(cfg == 0x20)
        {
            raw = raw & ~3; // 10 bit res, 187.5 ms
        }
        else if(cfg == 0x40)
        {
            raw = raw & ~1; // 11 bit res, 375 ms
        }
    }
    celsius = (float)raw / 16.0;
    fahrenheit = celsius * 1.8 + 32.0;
    if(LOG_USR_CHECK(USR_LOG_OW))
    {
        Serial.print("Temperature = ");
        Serial.print(celsius);
        Serial.print("C / ");
        Serial.print(fahrenheit);
        Serial.print("F");
    }

    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(100);
    digitalWrite(GREEN_LED_PIN, LOW);

    LOG_SYS("");
}

