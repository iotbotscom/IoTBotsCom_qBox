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


    Filename: IoTBotsCom_qBox_sflash.ino

    General Description: qBody Boards Serial Flash Example (SPI, W25Q64)

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
UINT8 sample_buf[] = {"SFlash Data\r\n"};
UINT8 data_buf[] = {"XXXXXXXXXXX\r\n"};
UINT32 mem_address = 0;

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
    LOG_SYS(" **** The qBox Serial FLASH Example **** ");
    LOG_SYS("***************************************************************************************************\r\n");

    // Log Man Setup Begin
    Logman_Get_Settings(&logging_mode);

    // Enable User Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_ENABLE);
    // Enable User Device Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_DEVICE);
    // Enable User SFlash Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_SFLASH);

    // Enable DBG Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_ENABLE);
    // Enable DBG SFlash Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_SPI);

    Logman_Set_Settings(&logging_mode);
    // Log Man Setup Begin End

#if ((defined ARDUINO_QBOARDA_ESP32) || (defined ARDUINO_QBOARDB_ESP32) || ((defined HW_PLATFORM_TYPE_STM32) && (HW_PLATFORM_TYPE_STM32 == TRUE)))
    // Load Enable Begin
    pinMode(LOAD_ENABLE_PIN, OUTPUT);
    digitalWrite(LOAD_ENABLE_PIN, HIGH);
    // Load Enable End

    delay(100);
#endif

    LOG_DEV("High Power On\r\n");

    // sFlash Begin
    SFLASH_Init();
    // sFlash End

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
    UINT16 i;
    UINT16 data_len;

    if(SFLASH_On() != TRUE)
    {
        LOG_DEV("sFlash is not enabled or detected\r\n");
    }
    else
    {
        digitalWrite(RED_LED_PIN, HIGH);

        LOG_DEV("sFlash found\r\n");

        data_len = strlen((char *)sample_buf);

        if(LOG_USR_CHECK(USR_LOG_DEVICE))
        {
            Serial.print("ADDR : 0x");
            Serial.print(mem_address, HEX);
            Serial.print(", LEN : 0x");
            Serial.println(data_len, HEX);
        }

        if(SFLASH_Write(mem_address, sample_buf, data_len) != TRUE)
        {
            LOG_DEV("sFlash_Write Failed\r\n");
        }

        if(SFLASH_Read(mem_address, data_buf, data_len) != TRUE)
        {
            LOG_DEV("sFlash_Read Failed\r\n");
        }

        for(i = 0; i < data_len; i++)
        {
            if(sample_buf[i] != data_buf[i])
            {
                LOG_DEV("Data Check Failed - To be Erasured\r\n");

                digitalWrite(GREEN_LED_PIN, HIGH);

                delay(100);

                SFLASH_Bulk_Erasure();

                digitalWrite(GREEN_LED_PIN, LOW);

                break;
            }
        }

        mem_address += 1000;

        if(mem_address >= 1000000)
        {
            mem_address = 10;
        }

        if(SFLASH_Off() == TRUE)
        {
            LOG_DEV("sFlash is Off\r\n");

            digitalWrite(RED_LED_PIN, LOW);
        }
        else
        {
            LOG_DEV("sFlash Off Failed\r\n");
        }
    }

    SCI_Processor();

    LOG_SYS("");

    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLUE_LED_PIN, LOW);

    delay(800);
}
