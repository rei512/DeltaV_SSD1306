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

void test();

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
    printf("SSD1306_WIDTH:%d\r\n", SSD1306_WIDTH);
    printf("SSD1306_HEIGHT:%d\r\n", SSD1306_HEIGHT);
    uint8_t str[] = "Ω1μ1℃01";
    for (int i = 0; i < sizeof(str); i++)
    {
        printf("str[%d]:0x%x\r\n", i, str[i]);
    }

    SSD1306_Init();
    SSD1306_Clear();
    SSD1306_Update();
    // ssdtest2();
    // SSD1306_Update();
    // Delay_Ms(500);

    while (1)
    {
        test();
        Delay_Ms(500);
    }
}

void PixelTest()
{
    SSD1306_Clear();
    SSD1306_DrawPixel(0, 0, 1);
    SSD1306_DrawPixel(SSD1306_WIDTH - 1, 0, 1);
    SSD1306_DrawPixel(0, SSD1306_HEIGHT - 1, 1);
    SSD1306_DrawPixel(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 1);
    SSD1306_Update();
}

void LineTest()
{
    SSD1306_Clear();
    SSD1306_DrawLine(0, 0, 0, SSD1306_HEIGHT - 1, 1);
    SSD1306_Update();
    SSD1306_DrawLine(0, SSD1306_HEIGHT - 1, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 1);
    SSD1306_Update();
    SSD1306_DrawLine(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_WIDTH - 1, 0, 1);
    SSD1306_Update();
    SSD1306_DrawLine(SSD1306_WIDTH - 1, 0, 0, 0, 1);
    SSD1306_Update();
    SSD1306_DrawLine(0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 1);
    SSD1306_Update();
    SSD1306_DrawLine(0, SSD1306_HEIGHT - 1, SSD1306_WIDTH - 1, 0, 1);
    SSD1306_Update();

    Delay_Ms(500);

    SSD1306_Clear();
    for (int i = 0; i <= SSD1306_WIDTH; i += 4)
    {
        SSD1306_DrawLine(0, 0, i, SSD1306_HEIGHT - 1, 1);
        SSD1306_Update();
    }
    for (int i = SSD1306_HEIGHT - 1; i > 0; i -= 4)
    {
        SSD1306_DrawLine(0, 0, SSD1306_WIDTH - 1, i, 1);
        SSD1306_Update();
    }
    Delay_Ms(500);

    SSD1306_Clear();
    for (int i = 0; i <= SSD1306_HEIGHT; i += 4)
    {
        SSD1306_DrawLine(SSD1306_WIDTH - 1, 0, 0, i, 1);
        SSD1306_Update();
    }
    for (int i = 0; i <= SSD1306_WIDTH; i += 4)
    {
        SSD1306_DrawLine(SSD1306_WIDTH - 1, 0, i, SSD1306_HEIGHT - 1, 1);
        SSD1306_Update();
    }
    Delay_Ms(500);

    SSD1306_Clear();
    for (int i = SSD1306_WIDTH - 1; i > 0; i -= 4)
    {
        SSD1306_DrawLine(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, i, 0, 1);
        SSD1306_Update();
    }
    for (int i = 0; i <= SSD1306_HEIGHT; i += 4)
    {
        SSD1306_DrawLine(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 0, i, 1);
        SSD1306_Update();
    }
    Delay_Ms(500);

    SSD1306_Clear();
    for (int i = SSD1306_HEIGHT - 1; i > 0; i -= 4)
    {
        SSD1306_DrawLine(0, SSD1306_HEIGHT - 1, SSD1306_WIDTH - 1, i, 1);
        SSD1306_Update();
    }
    for (int i = SSD1306_WIDTH - 1; i > 0; i -= 4)
    {
        SSD1306_DrawLine(0, SSD1306_HEIGHT - 1, i, 0, 1);
        SSD1306_Update();
    }
}

void RectTest()
{
    SSD1306_Clear();
    for (int16_t i = 0; i < SSD1306_HEIGHT / 2; i += 2)
    {
        SSD1306_DrawRect(i, i, SSD1306_WIDTH - 2 * i, SSD1306_HEIGHT - 2 * i, 1);
        SSD1306_Update();
    }

    Delay_Ms(500);

    SSD1306_Clear();
    for (int16_t i = 0; i < SSD1306_HEIGHT / 2; i += 3)
    {
        SSD1306_FillRect(i, i, SSD1306_WIDTH - 2 * i, SSD1306_HEIGHT - 2 * i, i % 2);
        SSD1306_Update();
    }
}

void CircleTest()
{
    SSD1306_Clear();
    int16_t i;
    for (i = 0; i < fmax(SSD1306_WIDTH, SSD1306_HEIGHT) / 2; i += 2)
    {
        SSD1306_DrawCircle(SSD1306_WIDTH / 2, SSD1306_HEIGHT / 2, i, 1);
        SSD1306_Update();
    }
    Delay_Ms(500);

    SSD1306_Clear();
    i = fmax(SSD1306_WIDTH, SSD1306_HEIGHT) / 2;
    for (; i > 0; i -= 3)
    {
        // The INVERSE color is used so circles alternate white/black
        SSD1306_FillCircle(SSD1306_WIDTH / 2, SSD1306_HEIGHT / 2, i, i % 2);
        SSD1306_Update();
    }
}

void RoundRectTest()
{
    SSD1306_Clear();
    for (int16_t i = 0; i < SSD1306_HEIGHT / 2 - 2; i += 2)
    {
        SSD1306_DrawRoundRect(i, i, SSD1306_WIDTH - 2 * i, SSD1306_HEIGHT - 2 * i, SSD1306_HEIGHT / 4, 1);
        SSD1306_Update();
    }

    Delay_Ms(500);

    SSD1306_Clear();
    for (int16_t i = 0; i < SSD1306_HEIGHT / 2 - 2; i += 3)
    {
        SSD1306_FillRoundRect(i, i, SSD1306_WIDTH - 2 * i, SSD1306_HEIGHT - 2 * i, SSD1306_HEIGHT / 4, i % 2);
        SSD1306_Update();
    }
}

void EllipseTest()
{
    SSD1306_Clear();
    for (int16_t i = 0; i < SSD1306_HEIGHT / 2 - 2; i += 2)
    {
        // SSD1306_DrawRect(i, i, SSD1306_WIDTH - 2 * i, SSD1306_HEIGHT - 2 * i, 1);
        SSD1306_DrawEllipse(i, i, SSD1306_WIDTH - 2 * i, SSD1306_HEIGHT - 2 * i, 1);
        SSD1306_Update();
    }

    Delay_Ms(500);

    SSD1306_Clear();
    for (int16_t i = 0; i < SSD1306_HEIGHT / 2 - 2; i += 3)
    {
        SSD1306_FillEllipse(i, i, SSD1306_WIDTH - 2 * i, SSD1306_HEIGHT - 2 * i, i % 2);
        SSD1306_Update();
    }
}

void TriangleTest()
{
    int i;
    SSD1306_Clear();

    for (i = 0; i < fmax(SSD1306_WIDTH, SSD1306_HEIGHT) / 2; i += 5)
    {
        SSD1306_DrawTriangle(
            SSD1306_WIDTH / 2, SSD1306_HEIGHT / 2 - i,
            SSD1306_WIDTH / 2 - i, SSD1306_HEIGHT / 2 + i,
            SSD1306_WIDTH / 2 + i, SSD1306_HEIGHT / 2 + i, 1);
        SSD1306_Update();
    }
    Delay_Ms(500);

    SSD1306_Clear();

    i = fmax(SSD1306_WIDTH, SSD1306_HEIGHT) / 2;
    for (; i > 0; i -= 5)
    {
        // The INVERSE color is used so triangles alternate white/black
        SSD1306_FillTriangle(
            SSD1306_WIDTH / 2, SSD1306_HEIGHT / 2 - i,
            SSD1306_WIDTH / 2 - i, SSD1306_HEIGHT / 2 + i,
            SSD1306_WIDTH / 2 + i, SSD1306_HEIGHT / 2 + i, i % 2);
        SSD1306_Update();
    }
}

void StringTest()
{
    // Test 1: ASCII Characters (All printable characters 32-126)
    SSD1306_Clear();
    SSD1306_DrawString(0,  0, "ASCII CHARACTERS", 1);
    SSD1306_DrawString(0,  8, " !\"#$%&'()*+,-./", 1);
    SSD1306_DrawString(0, 16, "0123456789:;<=>?", 1);
    SSD1306_DrawString(0, 24, "@ABCDEFGHIJKLMNO", 1);
    SSD1306_DrawString(0, 32, "PQRSTUVWXYZ[\\]^_", 1);
    SSD1306_DrawString(0, 40, "`abcdefghijklmno", 1);
    SSD1306_DrawString(0, 48, "pqrstuvwxyz{|}~ ", 1);
    SSD1306_Update();
    Delay_Ms(5000);

    // Test 2: Extended UTF-8 Characters
    SSD1306_Clear();
    SSD1306_DrawString(0,  0, "UTF-8 EXTENDED:", 1);
    SSD1306_DrawString(0,  8, "Greek Letters:", 1);
    SSD1306_DrawString(0, 16, "ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠ", 1);
    SSD1306_DrawString(0, 24, "ΡΣΤΥΦΧΨΩ", 1);
    SSD1306_DrawString(0, 32, "αβγδεζηθικλμνξοπ", 1);
    SSD1306_DrawString(0, 40, "ρστυφχψω", 1);
    SSD1306_DrawString(0, 48, "Special Symbols:", 1);
    SSD1306_DrawString(0, 56, "℃", 1);
    SSD1306_Update();
    Delay_Ms(5000);
    
    // Test 3: Use Cases (Practical Examples)
    SSD1306_Clear();
    SSD1306_DrawString(0,  0, "USE CASES:", 1);
    SSD1306_DrawString(0,  8, "Temp: 23.5℃", 1);
    SSD1306_DrawString(0, 16, "Resistance: 4.7Ω", 1);
    SSD1306_DrawString(0, 24, "Current: 150μA", 1);
    SSD1306_DrawString(0, 32, "Pi = 3.14159π", 1);
    SSD1306_DrawString(0, 40, "Formula: α+β=γ", 1);
    SSD1306_Update();
    Delay_Ms(3000);
}

void test(void)
{
    // PixelTest();
    // Delay_Ms(500);
    // LineTest();
    // Delay_Ms(500);
    // RectTest();
    // Delay_Ms(500);
    // CircleTest();
    // Delay_Ms(500);
    // EllipseTest();
    // Delay_Ms(500);
    // RoundRectTest();
    // Delay_Ms(500);
    // TriangleTest();
    // Delay_Ms(500);
    StringTest();
}