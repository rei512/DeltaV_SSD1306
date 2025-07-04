#ifndef __SSD1306_H
#define __SSD1306_H

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8) // 1024 bytes for 128x64 OLED

#define SSD1306_WRITE 0x00
#define SSD1306_READ 0x01

#define SSD1306_MODE_COMMAND 0x00 // Command mode
#define SSD1306_MODE_DATA 0x40    // Data mode

#define SSD1306_ADDRESS 0x3C // I2C address for SSD1306 SA0=0
// #define SSD1306_ADDRESS 0x3D // I2C address for SSD1306 SA0=1

// SSD1306 Commands
// Charge Pump Command Table
#define SSD1306_CMD_SET_CHARGE_PUMP 0x8D // Set Charge Pump 1byte: 0x10=Disable Charge Pump, 0x14=Enable Charge Pump (RESET)

// Fundamental Command Table
#define SSD1306_CMD_SET_CONTRAST 0x81 // Set Contrast Control 1byte(contrast value)

#define SSD1306_CMD_SET_DISPLAY_ALL_NORMAL 0xA4
#define SSD1306_CMD_SET_DISPLAY_ALL_ON 0xA5 // Entire Display ON

#define SSD1306_CMD_SET_NORMAL_DISPLAY 0xA6
#define SSD1306_CMD_SET_INVERSE_DISPLAY 0xA7 // Set Normal/Inverse Display

#define SSD1306_CMD_SET_DISPLAY_OFF 0xAE
#define SSD1306_CMD_SET_DISPLAY_ON 0xAF // Set Display ON/OFF

// Scrolling Command Table
#define SSD1306_CMD_SET_SCROLL_HORIZONTAL_RIGHT 0x26 // Continuous Horizontal Scroll Setup 6byte: 0x00 + start PAGE address(0-7) + scroll speed(0-7) + end PAGE address(0-7) + 0x00 + 0xFF
#define SSD1306_CMD_SET_SCROLL_HORIZONTAL_LEFT 0x27

#define SSD1306_CMD_SET_SCROLL_VERTICAL_RIGHT 0x29 // Continuous Vertical and Horizontal Scroll Setup 5byte: 0x00 + start PAGE address(0-7) + scroll speed(0-7) + end PAGE address(0-7) + vertical scrolling offset (0-43)
#define SSD1306_CMD_SET_SCROLL_VERTICAL_LEFT 0x2A

#define SSD1306_CMD_SET_SCROLL_STOP 0x2E  // Deactivate scroll
#define SSD1306_CMD_SET_SCROLL_START 0x2F // Activate scroll

#define SSD1306_CMD_SET_SCROLL_VERTICAL_AREA 0xA3 // Set Vertical Scroll Area 2byte: top margin (0-63) + bottom margin (0-63)

// Addressing Setting Command Table
#define SSD1306_CMD_SET_LOWER_COLUMN_ADDRESS_OFFSET 0x00  // Set Lower Column Start Address for Page Addressing Mode (0-15)
#define SSD1306_CMD_SET_HIGHER_COLUMN_ADDRESS_OFFSET 0x10 // Set Higher Column Start Address for Page Addressing Mode (0-15)
#define SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE 0x20       // Set Memory Addressing Mode 1byte: 0x00=Horizontal, 0x01=Vertical, 0x02=Page Addressing Mode(RESET)
#define SSD1306_CMD_SET_COLUMN_ADDRESS 0x21               // Set Column Address 2byte: start column address(0-127) + end column address(0-127)
#define SSD1306_CMD_SET_PAGE_ADDRESS 0x22                 // Set Page Address 2byte: start page address(0-7) + end page address(0-7)
#define SSD1306_CMD_SET_PAGE_START_ADDRESS_OFFSET 0xB0    // Set Page Start Address for Page Addressing Mode (0-7)

// Hardware Configuration (Panel resolution & layout related) Command Table
#define SSD1306_CMD_SET_DISPLAY_START_LINE_OFFSET 0x40       // Set display RAM display start line register (0-63)
#define SSD1306_CMD_SET_SEGMENT_REMAP_NORMAL 0xA0            // Set Segment Re-map (0xA0=Column Address 0 mapped to SEG0, 0xA1=Column Address 127 mapped to SEG0)
#define SSD1306_CMD_SET_SEGMENT_REMAP_INVERSE 0xA1           // Inverse Segment Re-map
#define SSD1306_CMD_SET_MULTIPLEX_RATIO 0xA8                 // Set MUX ratio to N+1 MUX 1byte: N=15-63
#define SSD1306_CMD_SET_COM_SCAN_DIRECTION_NORMAL 0xC0       // Set COM Output Scan Direction normal mode (RESET) Scan from COM0 to COM[N –1]
#define SSD1306_CMD_SET_COM_SCAN_DIRECTION_INVERSE 0xC8      // Inverse COM Output Scan Direction remapped mode. Scan from COM[N-1] to COM0 Where N is the Multiplex ratio.
#define SSD1306_CMD_SET_DISPLAY_OFFSET 0xD3                  // Set Display Offset 1byte: offset value (0-63) to shift the display vertically
#define SSD1306_CMD_SET_COM_PINS_HARDWARE_CONFIGURATION 0xDA // Set COM Pins Hardware Configuration 1byte: 0x02=Alternative COM pin configuration, 0x12=Default COM pin configuration, 0x22=Default COM pin configuration (RESET),  0x32=Alternative COM pin configuration

// Timing & Driving Scheme Setting Command Table
#define SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO 0xD5 // Set Display 1byte: divide ratio (0-15) and oscillator frequency (0-15) << 4
#define SSD1306_CMD_SET_PRECHARGE_PERIOD 0xD9           // Set Pre-charge Period 2byte: Phase 1 period (0-15) + Phase 2 period (0-15)<<4
#define SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL 0xDB       // Set VCOMH Deselect Level 1byte: 0x00=0.65*VCC, 0x20=0.77*VCC, 0x30=0.83*VCC (RESET), 0x40=0.90*VCC, 0x50=0.95*VCC
#define SSD1306_CMD_SET_DEEP_SLEEP_MODE 0xE2            // Set Deep Sleep Mode 1byte: 0x01=Enter Deep Sleep Mode, 0x00=Exit Deep Sleep Mode (RESET)
#define SSD1306_CMD_SET_NOP 0xE3                        // No Operation Command

/**
 * @brief スワップ用のダブルバッファを切り替える
 * @note 滑らかなアニメーションを実現するために使用
 */
void SSD1306_Buffer_swap(void);

/**
 * @brief SSD1306 OLEDディスプレイを初期化する
 * @note I2C通信とディスプレイの基本設定を行う
 */
void SSD1306_Init(void);

/**
 * @brief ディスプレイバッファをクリアする（全ピクセルを消去）
 */
void SSD1306_Clear(void);

/**
 * @brief バッファの内容をディスプレイに転送する
 * @note 描画操作後に必ず呼び出す必要がある
 */
void SSD1306_Update(void);

/**
 * @brief バッファの全体をディスプレイに強制転送する
 */
void SSD1306_Update_Full(void);

/**
 * @brief ディスプレイをオンにする
 */
void SSD1306_DisplayOn(void);

/**
 * @brief ディスプレイをオフにする（省電力モード）
 */
void SSD1306_DisplayOff(void);

/**
 * @brief 全ピクセルを点灯させる（テスト用）
 */
void SSD1306_DisplayAllOn(void);

/**
 * @brief 通常の表示モードに戻す
 */
void SSD1306_DisplayNormal(void);

/**
 * @brief 表示を反転させる（白黒反転）
 */
void SSD1306_DisplayInverse(void);

/**
 * @brief ディスプレイのコントラストを設定する
 * @param contrast コントラスト値 (0-255)
 */
void SSD1306_SetContrast(uint8_t contrast);

/**
 * @brief 指定座標にピクセルを描画する
 * @param x X座標 (0-127)
 * @param y Y座標 (0-63)
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color);

/**
 * @brief 2点間に直線を描画する
 * @param x0 開始点のX座標
 * @param y0 開始点のY座標
 * @param x1 終了点のX座標
 * @param y1 終了点のY座標
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);

/**
 * @brief 矩形の枠線を描画する
 * @param x 左上角のX座標
 * @param y 左上角のY座標
 * @param width 幅
 * @param height 高さ
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);

/**
 * @brief 塗りつぶされた矩形を描画する
 * @param x 左上角のX座標
 * @param y 左上角のY座標
 * @param width 幅
 * @param height 高さ
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
/**
 * @brief 円の枠線を描画する
 * @param x0 中心のX座標
 * @param y0 中心のY座標
 * @param radius 半径
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color);

/**
 * @brief 塗りつぶされた円を描画する
 * @param x0 中心のX座標
 * @param y0 中心のY座標
 * @param radius 半径
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_FillCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color);
/**
 * @brief 楕円の枠線を描画する
 * @param x0 バウンディングボックス左上のX座標
 * @param y0 バウンディングボックス左上のY座標
 * @param x1 バウンディングボックス右下のX座標
 * @param y1 バウンディングボックス右下のY座標
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawEllipse(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);

/**
 * @brief 塗りつぶされた楕円を描画する
 * @param x0 バウンディングボックス左上のX座標
 * @param y0 バウンディングボックス左上のY座標
 * @param x1 バウンディングボックス右下のX座標
 * @param y1 バウンディングボックス右下のY座標
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_FillEllipse(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
/**
 * @brief 三角形の枠線を描画する
 * @param x0 第1頂点のX座標
 * @param y0 第1頂点のY座標
 * @param x1 第2頂点のX座標
 * @param y1 第2頂点のY座標
 * @param x2 第3頂点のX座標
 * @param y2 第3頂点のY座標
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);

/**
 * @brief 塗りつぶされた三角形を描画する
 * @param x0 第1頂点のX座標
 * @param y0 第1頂点のY座標
 * @param x1 第2頂点のX座標
 * @param y1 第2頂点のY座標
 * @param x2 第3頂点のX座標
 * @param y2 第3頂点のY座標
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_FillTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
/**
 * @brief 角の丸い矩形の枠線を描画する
 * @param x0 左上角のX座標
 * @param y0 左上角のY座標
 * @param x1 右下角のX座標
 * @param y1 右下角のY座標
 * @param radius 角の丸みの半径
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawRoundRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t radius, uint8_t color);

/**
 * @brief 角の丸い塗りつぶされた矩形を描画する
 * @param x0 左上角のX座標
 * @param y0 左上角のY座標
 * @param x1 右下角のX座標
 * @param y1 右下角のY座標
 * @param radius 角の丸みの半径
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_FillRoundRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t radius, uint8_t color);

/**
 * @brief ASCII文字を描画する
 * @param x 描画開始のX座標
 * @param y 描画開始のY座標
 * @param c 描画する文字
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawChar(uint8_t x, uint8_t y, char c, uint8_t color);

/**
 * @brief UTF-8文字を描画する（ギリシャ文字等対応）
 * @param x 描画開始のX座標
 * @param y 描画開始のY座標
 * @param utf8_bytes UTF-8バイト列のポインタ
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawCharUTF8(uint8_t x, uint8_t y, const uint8_t *utf8_bytes, uint8_t color);

/**
 * @brief 文字列を描画する（UTF-8対応）
 * @param x 描画開始のX座標
 * @param y 描画開始のY座標
 * @param str 描画する文字列のポインタ
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawString(uint8_t x, uint8_t y, const char *str, uint8_t color);

/**
 * @brief ビットマップ画像を描画する
 * @param x 描画開始のX座標
 * @param y 描画開始のY座標
 * @param bitmap ビットマップデータのポインタ
 * @param width ビットマップの幅
 * @param height ビットマップの高さ
 * @param color 色 (0=消去, 1=点灯)
 */
void SSD1306_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, uint8_t color);


#endif
