#ifndef __SSD1306_HAL_H
#define __SSD1306_HAL_H

#include <stdint.h>

#define SSD1306_SCL GPIO_Pin_16
#define SSD1306_SDA GPIO_Pin_17

/**
 * @brief ミリ秒単位の遅延処理（HAL層）
 * @param ms 遅延時間（ミリ秒）
 * @note マイコン固有の遅延関数を呼び出す
 */
void SSD1306_Delay_Ms_HAL(uint32_t ms);

/**
 * @brief I2C通信の初期化（HAL層）
 * @note マイコン固有のI2C初期化処理を実行
 */
void SSD1306_IIC_Init_HAL(void);

/**
 * @brief I2C通信でデータを送信（HAL層）
 * @param Mode 送信モード (SSD1306_MODE_COMMAND または SSD1306_MODE_DATA)
 * @param Command 送信するデータのポインタ
 * @param Length 送信するデータの長さ（バイト数）
 * @note マイコン固有のI2C送信処理を実行
 */
void SSD1306_IIC_HAL(uint8_t Mode, uint8_t *Command, uint8_t Length);
#endif
