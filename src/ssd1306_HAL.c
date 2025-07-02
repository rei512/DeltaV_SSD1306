#include "debug.h"

#include "ssd1306_HAL.h"
#include "ssd1306.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */

void SSD1306_Delay_Ms_HAL(uint32_t ms)
{
    Delay_Ms(ms);
}

void SSD1306_IIC_Init_HAL(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    GPIO_PinRemapConfig(GPIO_PartialRemap2_I2C1, ENABLE);
    // C16-SDA, C17-SCL
    GPIO_InitStructure.GPIO_Pin = SSD1306_SCL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SSD1306_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2C_InitStructure.I2C_ClockSpeed = 800000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);

    I2C_AcknowledgeConfig(I2C1, ENABLE);
}

void SSD1306_IIC_HAL(uint8_t Mode, uint8_t *Command, uint8_t Length)
{
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
        ;
    I2C_GenerateSTART(I2C1, ENABLE);
    // printf("I2C Start\r\n");
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    ;
    I2C_Send7bitAddress(I2C1, SSD1306_ADDRESS << 1, I2C_Direction_Transmitter);
    // printf("I2C Send Address\r\n");
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    // printf("I2C Wait for Address\r\n");
    // if (I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) != RESET)
    // {
    //     I2C_SendData(I2C1, Command[0]);
    //     // printf("I2C Send Command: 0x%02X\r\n", Command[0]);
    // }
    I2C_SendData(I2C1, Mode);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    for (int i = 0; i < Length; i++)
    {
        I2C_SendData(I2C1, Command[i]);
        while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            ;
    }

    // while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    //     ;
    I2C_GenerateSTOP(I2C1, ENABLE);
    // printf("I2C Stop\r\n");
}