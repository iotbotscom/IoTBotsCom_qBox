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


    Filename: IoTBotsCom_qBox_rs485_master.ino

    General Description: qBody Boards RS485 Master Example

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
PBUF_T tx;
UINT8 buf_tx[128];
PBUF_T rx;
UINT8 buf_rx[128];

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
    LOG_SYS(" **** The qBox RS485 Slave Example **** ");
    LOG_SYS("***************************************************************************************************\r\n");

    // Log Man Setup Begin
    Logman_Get_Settings(&logging_mode);

    // Enable User Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_ENABLE);
    // Enable User Device Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_DEVICE);
    // Enable User RS485 Logging
    logging_mode.logging_usr |= (1 << (UINT32)USR_LOG_RS485);

    // Enable DBG Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_ENABLE);
    // Enable DBG Device Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_DEVICE);
    // Enable DBG RS485 Logging
    logging_mode.logging_dbg |= (1 << (UINT32)DBG_LOG_RS485);

    Logman_Set_Settings(&logging_mode);
    // Log Man Setup Begin End

#if ((defined ARDUINO_QBOARDA_ESP32) || (defined ARDUINO_QBOARDB_ESP32))
    // Load Enable Begin
    pinMode(LOAD_ENABLE_PIN, OUTPUT);
    digitalWrite(LOAD_ENABLE_PIN, HIGH);

    delay(100);
    // Load Enable End
#endif


    rx.buf = buf_rx;
    rx.len = 0;
    rx.buf[rx.len] = 0;

    tx.buf = buf_tx;
    tx.len = 0;
    tx.buf[tx.len] = 0;

    // RS485 Begin
    RS485_Init();

    RS485_On();
    // RS485 End

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
    UINT8 i;

    // LLS RX : 3e,01,06,19,59,07,00,00,74
    tx.len = 0;
    tx.buf[tx.len++] = 0x3e;
    tx.buf[tx.len++] = 0x01;
    tx.buf[tx.len++] = 0x06;
    tx.buf[tx.len++] = 0x19;
    tx.buf[tx.len++] = 0x59;
    tx.buf[tx.len++] = 0x07;
    tx.buf[tx.len++] = 0x00;
    tx.buf[tx.len++] = 0x00;
    tx.buf[tx.len++] = 0x74;

    if(RS485_Recv(&rx, 1000) == TRUE)
    {
        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);

        if(LOG_USR_CHECK(USR_LOG_DEVICE))
        {
            Serial.print("RX: ");
            
            for(i = 0; i < rx.len; i++)
            {
                Serial.print("0x");
                Serial.print(rx.buf[i], HEX);
                Serial.print(" ");
            }
            Serial.print(", ");
            Serial.print(rx.len);
            Serial.println("");
        }

        if(rx.len == 4
            && (rx.buf[0] == 0x31 && rx.buf[1] == 0x01 && rx.buf[2] == 0x06 && rx.buf[3] == 0x6c))
        {
            if(RS485_Send(&tx) == TRUE)
            {
                if(LOG_USR_CHECK(USR_LOG_DEVICE))
                {
                    Serial.print("TX: ");
                    
                    for(i = 0; i < tx.len; i++)
                    {
                        Serial.print("0x");
                        Serial.print(tx.buf[i], HEX);
                        Serial.print(" ");
                    }
                    Serial.print(", ");
                    Serial.print(tx.len);
                    Serial.println("");
                }

                digitalWrite(GREEN_LED_PIN, HIGH);
                delay(100);
                digitalWrite(GREEN_LED_PIN, LOW);
            }
        }
        else
        {
            LOG_DEV("Wrong Data Received\r\n");
        }
    }

    SCI_Processor();

    LOG_SYS("");

    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLUE_LED_PIN, LOW);
    delay(100);
}

