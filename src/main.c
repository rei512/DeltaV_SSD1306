/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/04/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *7-bit addressing mode, master/slave mode, transceiver routine:
 *I2C1_SCL(PA10)\I2C1_SDA(PA11).
 *This routine demonstrates that Master sends and Slave receives.
 *Note: The two boards download the Master and Slave programs respectively,
 *and power on at the same time.During the I2C communication process,
 *the pins are open drain outputs.
 *    Hardware connection:PA10 -- PA10
 *                        PA11 -- PA11
 *
 */

#include "debug.h"
#include "ssd1306.h"
#include "ssd1306_HAL.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("SSD1306 OLED Test\r\n");

    SSD1306_Init();
    SSD1306_Clear();
    SSD1306_Update();

    
        test();
        SSD1306_Update();
        Delay_Ms(1000);

    while (1)
    {
        SSD1306_DisplayInverse();
        Delay_Ms(1000);
        SSD1306_DisplayNormal();
        Delay_Ms(1000);
    }
}
