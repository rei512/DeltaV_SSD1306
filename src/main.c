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

#include <math.h>

asm(".global _printf_float");

void FunctionTest();
void smooth_animation();
void ClockTest();

void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
}

void TIM1_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    // TIM_CtrlPWMOutputs(TIM1, ENABLE );
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

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
    GPIO_Toggle_INIT();
    TIM1_Init(65535 - 1, 4800 - 1, 50);

    SSD1306_Init();

    while (1)
    {
        FunctionTest();
        Delay_Ms(3000);

        TIM1->CNT = 0;
        smooth_animation();
        int cnt = TIM1->CNT;
        printf("smooth_animation:%.1fms, %.2fHz\r\n", cnt / 10.0f, 200.0f / cnt * 10000.0f);
        Delay_Ms(500);

        TIM1->CNT = 0;
        ClockTest();
        printf("ClockTest:%.1fms\r\n", TIM1->CNT / 10.0f);
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

    // Delay_Ms(500);

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
    // Delay_Ms(500);

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
    // Delay_Ms(500);

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
    // Delay_Ms(500);

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

    // Delay_Ms(500);

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
    // Delay_Ms(500);

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

    // Delay_Ms(500);

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

    // Delay_Ms(500);

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
    // Delay_Ms(500);

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
    SSD1306_DrawString(0, 0, "ASCII CHARACTERS", 1);
    SSD1306_DrawString(0, 8, " !\"#$%&'()*+,-./", 1);
    SSD1306_DrawString(0, 16, "0123456789:;<=>?", 1);
    SSD1306_DrawString(0, 24, "@ABCDEFGHIJKLMNO", 1);
    SSD1306_DrawString(0, 32, "PQRSTUVWXYZ[\\]^_", 1);
    SSD1306_DrawString(0, 40, "`abcdefghijklmno", 1);
    SSD1306_DrawString(0, 48, "pqrstuvwxyz{|}~ ", 1);
    SSD1306_Update();
    Delay_Ms(1000);

    // Test 2: Extended UTF-8 Characters
    SSD1306_Clear();
    SSD1306_DrawString(0, 0, "UTF-8 EXTENDED:", 1);
    SSD1306_DrawString(0, 8, "Greek Letters:", 1);
    SSD1306_DrawString(0, 16, "ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠ", 1);
    SSD1306_DrawString(0, 24, "ΡΣΤΥΦΧΨΩ", 1);
    SSD1306_DrawString(0, 32, "αβγδεζηθικλμνξοπ", 1);
    SSD1306_DrawString(0, 40, "ρστυφχψω", 1);
    SSD1306_DrawString(0, 48, "Special Symbols:", 1);
    SSD1306_DrawString(0, 56, "℃", 1);
    SSD1306_Update();
    Delay_Ms(1000);

    // Test 3: Use Cases (Practical Examples)
    SSD1306_Clear();
    SSD1306_DrawString(0, 0, "USE CASES:", 1);
    SSD1306_DrawString(0, 8, "Temp: 23.5℃", 1);
    SSD1306_DrawString(0, 16, "Resistance: 4.7Ω", 1);
    SSD1306_DrawString(0, 24, "Current: 150μA", 1);
    SSD1306_DrawString(0, 32, "Pi = 3.14159π", 1);
    SSD1306_DrawString(0, 40, "Formula: α+β=γ", 1);
    SSD1306_Update();
    Delay_Ms(1000);
}

void FunctionTest(void)
{
    TIM1->CNT = 0;
    Delay_Ms(1);
    printf("Delay:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    Delay_Ms(1);
    printf("Delay:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    PixelTest();
    printf("PixelTest:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    LineTest();
    printf("LineTest:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    RectTest();
    printf("RectTest:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    CircleTest();
    printf("CircleTest:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    EllipseTest();
    printf("EllipseTest:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    RoundRectTest();
    printf("RoundRectTest:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    TriangleTest();
    printf("TriangleTest:%.1fms\r\n", TIM1->CNT / 10.0f);
    Delay_Ms(500);

    TIM1->CNT = 0;
    StringTest();
    printf("StringTest:%.1fms\r\n", TIM1->CNT / 10.0f);
}

void smooth_animation(void)
{
    int frame = 0;
    uint8_t str[8];
    while (frame++ < 600)
    { // Clear drawing buffer

        SSD1306_Clear();

        // Draw multiple animated objects
        int x = 64 + 30 * sinf(frame * 0.05f);
        int y = 32 + 20 * cosf(frame * 0.05f);

        SSD1306_FillCircle(x, y, 8, 1);
        SSD1306_DrawRect(20, 20, 80, 30, 1);
        sprintf((char *)str, "Frame: %d", frame);
        SSD1306_DrawString(0, 0, (const char *)str, 1);
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);

        // Single optimized update
        SSD1306_Update(); // Only changed regions transmitted
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
    }
}

void ClockTest()
{
    // Static variables to maintain time state
    static uint8_t hours = 12;
    static uint8_t minutes = 30;
    static uint8_t seconds = 0;
    static uint32_t last_update = 0;

    // Time constants
    const uint8_t analog_center_x = 96; // Right side center
    const uint8_t analog_center_y = 32; // Middle
    const uint8_t analog_radius = 30;   // Clock face radius

    // Update time every second (simulated)
    uint32_t current_time = SystemCoreClock / 1000; // Approximate milliseconds
    if (current_time - last_update > 1000)
    {
        seconds++;
        if (seconds >= 60)
        {
            seconds = 0;
            minutes++;
            if (minutes >= 60)
            {
                minutes = 0;
                hours++;
                if (hours > 12)
                {
                    hours = 1;
                }
            }
        }
        last_update = current_time;
    }

    SSD1306_Clear();

    // === DIGITAL CLOCK (Left Side) ===
    SSD1306_DrawString(0, 0, "DIGITAL", 1);
    SSD1306_DrawString(0, 8, " CLOCK", 1);

    // Update digital time display

    // Display date
    SSD1306_DrawString(0, 32, "2024", 1);
    SSD1306_DrawString(0, 40, "07/01", 1);
    SSD1306_DrawString(0, 48, "Monday", 1);

    // Temperature display with Celsius symbol
    SSD1306_DrawString(0, 56, "24.5℃", 1);

    // === ANALOG CLOCK (Right Side) ===
    // Draw clock face border
    SSD1306_DrawCircle(analog_center_x, analog_center_y, analog_radius, 1);

    // Draw hour markers (12, 3, 6, 9)
    for (int i = 0; i < 4; i++)
    {
        float angle = i * 90.0 * 3.14159 / 180.0; // Convert to radians
        int x1 = analog_center_x + (analog_radius - 3) * cos(angle);
        int y1 = analog_center_y + (analog_radius - 3) * sin(angle);
        int x2 = analog_center_x + (analog_radius - 1) * cos(angle);
        int y2 = analog_center_y + (analog_radius - 1) * sin(angle);
        SSD1306_DrawLine(x1, y1, x2, y2, 1);
    }

    // Draw minute markers (small dots)
    for (int i = 0; i < 12; i++)
    {
        if (i % 3 != 0)
        {                                                           // Skip hour markers
            float angle = i * 30.0 * 3.14159 / 180.0 - 3.14159 / 2; // -90° offset
            int x = analog_center_x + (analog_radius - 2) * cos(angle);
            int y = analog_center_y + (analog_radius - 2) * sin(angle);
            SSD1306_DrawPixel(x, y, 1);
        }
    }

    // Calculate hand angles (12 o'clock = -90 degrees)
    float hour_angle = ((hours % 12) * 30.0 + minutes * 0.5) * 3.14159 / 180.0 - 3.14159 / 2;
    float minute_angle = (minutes * 6.0) * 3.14159 / 180.0 - 3.14159 / 2;
    float second_angle = (seconds * 6.0) * 3.14159 / 180.0 - 3.14159 / 2;

    // Draw hour hand (thick, short)
    int hour_x = analog_center_x + (analog_radius - 12) * cos(hour_angle);
    int hour_y = analog_center_y + (analog_radius - 12) * sin(hour_angle);
    SSD1306_DrawLine(analog_center_x, analog_center_y, hour_x, hour_y, 1);
    SSD1306_DrawLine(analog_center_x - 1, analog_center_y, hour_x - 1, hour_y, 1); // Make thick

    // Draw minute hand (medium length)
    int minute_x = analog_center_x + (analog_radius - 6) * cos(minute_angle);
    int minute_y = analog_center_y + (analog_radius - 6) * sin(minute_angle);
    SSD1306_DrawLine(analog_center_x, analog_center_y, minute_x, minute_y, 1);

    // Draw second hand (thin, long)
    int second_x = analog_center_x + (analog_radius - 3) * cos(second_angle);
    int second_y = analog_center_y + (analog_radius - 3) * sin(second_angle);
    SSD1306_DrawLine(analog_center_x, analog_center_y, second_x, second_y, 1);

    // Draw center dot
    SSD1306_FillCircle(analog_center_x, analog_center_y, 2, 1);

    // Draw separator line between digital and analog
    SSD1306_DrawLine(64, 0, 64, 63, 1);

    // SSD1306_Update();

    // Animate for a few seconds to show time progression
    for (int i = 0; i < 1000; i++)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, 1);
        // Delay_Ms(200);

        // Display time in HH:MM:SS format
        char time_str[16];
        sprintf(time_str, "%02d:%02d:%02d", hours, minutes, seconds);
        SSD1306_FillRect(0, 16, 63, 23, 0); // Clear previous time
        SSD1306_DrawString(0, 16, time_str, 1);

        // Update seconds for animation
        seconds++;
        if (seconds >= 60)
        {
            seconds = 0;
            minutes++;
            if (minutes >= 60)
            {
                minutes = 0;
                hours++;
                if (hours > 12)
                {
                    hours = 1;
                }
            }
        }

        // Re-draw only the changing parts (optimization)
        // Clear previous hands area
        SSD1306_FillCircle(analog_center_x, analog_center_y, analog_radius - 2, 0);

        // Redraw clock face
        SSD1306_DrawCircle(analog_center_x, analog_center_y, analog_radius, 1);

        // Redraw markers
        for (int j = 0; j < 4; j++)
        {
            float angle = j * 90.0 * 3.14159 / 180.0;
            int x1 = analog_center_x + (analog_radius - 3) * cos(angle);
            int y1 = analog_center_y + (analog_radius - 3) * sin(angle);
            int x2 = analog_center_x + (analog_radius - 1) * cos(angle);
            int y2 = analog_center_y + (analog_radius - 1) * sin(angle);
            SSD1306_DrawLine(x1, y1, x2, y2, 1);
        }

        // Redraw minute markers
        for (int j = 0; j < 12; j++)
        {
            if (j % 3 != 0)
            {
                float angle = j * 30.0 * 3.14159 / 180.0 - 3.14159 / 2;
                int x = analog_center_x + (analog_radius - 2) * cos(angle);
                int y = analog_center_y + (analog_radius - 2) * sin(angle);
                SSD1306_DrawPixel(x, y, 1);
            }
        }

        // Recalculate and redraw hands
        hour_angle = ((hours % 12) * 30.0 + minutes * 0.5) * 3.14159 / 180.0 - 3.14159 / 2;
        minute_angle = (minutes * 6.0) * 3.14159 / 180.0 - 3.14159 / 2;
        second_angle = (seconds * 6.0) * 3.14159 / 180.0 - 3.14159 / 2;

        hour_x = analog_center_x + (analog_radius - 12) * cos(hour_angle);
        hour_y = analog_center_y + (analog_radius - 12) * sin(hour_angle);
        SSD1306_DrawLine(analog_center_x, analog_center_y, hour_x, hour_y, 1);
        SSD1306_DrawLine(analog_center_x - 1, analog_center_y, hour_x - 1, hour_y, 1);

        minute_x = analog_center_x + (analog_radius - 6) * cos(minute_angle);
        minute_y = analog_center_y + (analog_radius - 6) * sin(minute_angle);
        SSD1306_DrawLine(analog_center_x, analog_center_y, minute_x, minute_y, 1);

        second_x = analog_center_x + (analog_radius - 3) * cos(second_angle);
        second_y = analog_center_y + (analog_radius - 3) * sin(second_angle);
        SSD1306_DrawLine(analog_center_x, analog_center_y, second_x, second_y, 1);

        SSD1306_FillCircle(analog_center_x, analog_center_y, 2, 1);

        // Draw clock numbers (12, 3, 6, 9)
        SSD1306_DrawString(analog_center_x - 8, analog_center_y - analog_radius + 5, "12", 1);
        SSD1306_DrawString(analog_center_x + analog_radius - 11, analog_center_y - 3, "3", 1);
        SSD1306_DrawString(analog_center_x - 4, analog_center_y + analog_radius - 12, "6", 1);
        SSD1306_DrawString(analog_center_x - analog_radius + 4, analog_center_y - 4, "9", 1);
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
        SSD1306_Update();
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
    }
}