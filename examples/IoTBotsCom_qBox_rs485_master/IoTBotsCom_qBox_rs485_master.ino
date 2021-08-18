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
    LOG_SYS(" **** The qBox RS485 Master Example **** ");
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
    // LLS TX : 31,01,06,6c
    tx.len = 0;
    tx.buf[tx.len++] = 0x31;
    tx.buf[tx.len++] = 0x01;
    tx.buf[tx.len++] = 0x06;
    tx.buf[tx.len++] = 0x6c;

#if 1
    if(RS485_Trsf(&tx, &rx, 3000) == TRUE)
    {
        //LOG_DEV("Some Data Received\r\n");

        digitalWrite(GREEN_LED_PIN, HIGH);
        delay(100);
        digitalWrite(GREEN_LED_PIN, LOW);
    }
    else
    {
        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);

        LOG_DEV("No Data Received\r\n");
    }
#else
    if(RS485_Send(&tx) == TRUE)
    {
        digitalWrite(RED_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_LED_PIN, LOW);

        if(RS485_Recv(&rx, 3000) == TRUE)
        {
            //LOG_DEV("Some Data Received\r\n");

            digitalWrite(GREEN_LED_PIN, HIGH);
            delay(100);
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else
        {
            LOG_DEV("No Data Received\r\n");
        }
    }
    else
    {
        LOG_DEV("Data Sending Faield\r\n");
    }
#endif

    SCI_Processor();

    LOG_SYS("");

    delay(800);

    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLUE_LED_PIN, LOW);
}

