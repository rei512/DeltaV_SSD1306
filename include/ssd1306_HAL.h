#ifndef __SSD1306_HAL_H
#define __SSD1306_HAL_H

#include <stdint.h>

#define SSD1306_SCL GPIO_Pin_16
#define SSD1306_SDA GPIO_Pin_17

void SSD1306_Delay_Ms_HAL(uint32_t ms);

void SSD1306_IIC_Init_HAL(void);
void SSD1306_IIC_HAL(uint8_t Mode, uint8_t *Command, uint8_t Length);
#endif
