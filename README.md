# SSD1306 I2C OLED用軽量高速ライブラリ

汎用のSSD1306 OLEDディスプレイライブラリです。
128x64、128x32の1色OLEDに対応しています。
フラッシュ2kB、RAM2kBの超軽量
最大フレームレート200fps以上
HALによるロジック分離により極低オーバーヘッド、移植も容易
ASCII文字列、ギリシャ文字、℃に対応


## 基本的な使用方法


```c
#include "debug.h"
#include "ssd1306.h"

int main(void)
{
    // ここまでマイコンの初期化関数

    SSD1306_Init();

    SSD1306_DrawString(0, 0, "Hello World", 1);
    SSD1306_Update();
    int i = 0;
    char str[10];

    while (1)
    {
        SSD1306_FillRect(20, 20, 44, 28, 0);
        sprintf(str, "%d", i++);
        SSD1306_DrawString(20, 20, str, 1);
        SSD1306_Update();
        Delay_Ms(100);
    }
}
```

## ファイル構成
- `src/ssd1306_HAL.c` - ハードウェア抽象化層、使用するハードウェアに合わせてI2Cの実装を書いてください
- `src/ssd1306.c` - ディスプレイドライバ
- `include/ssd1306.h` - API定義
- `include/ssd1306_font.h` - フォントデータ

## 使い方
- `ssd1306_HAL.c`,`ssd1306.c`,`ssd1306.h`,`ssd1306_font.h`を使用するプロジェクトにコピー
- コードにて`ssd1306.h`をinclude

### 描画関数
- `SSD1306_DrawPixel()` - 点
- `SSD1306_DrawLine()` - 線
- `SSD1306_DrawRect()` / `SSD1306_FillRect()` - 矩形
- `SSD1306_DrawCircle()` / `SSD1306_FillCircle()` - 円
- `SSD1306_DrawEllipse()` / `SSD1306_FillEllipse()` - 楕円
- `SSD1306_DrawTriangle()` / `SSD1306_FillTriangle()` - 三角形

### 文字描画
- `SSD1306_DrawString()` - 文字列（UTF-8対応）
- ASCII文字列と特殊記号に対応

### 最適化機能
- ダブルバッファリング

## デモ

実装されているテスト関数：
- `FunctionTest()` - 図形描画テスト
- `smooth_animation()` - アニメーション
- `ClockTest()` - デジタル・アナログ時計

