# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a PlatformIO-based embedded C project for the CH32X035 microcontroller (WCH/QinHeng) with SSD1306 OLED display support. The project demonstrates I2C communication and graphics rendering on a 128x64 monochrome OLED display.

## Build and Development Commands

```bash
# Build the project
platformio run

# Build and upload to device
platformio run --target upload

# Clean build files
platformio run --target clean

# Monitor serial output
platformio device monitor

# Build for specific environment
platformio run -e ch32x035g8u6_evt_r0
```

## Hardware Configuration

- **Target MCU**: CH32X035G8U6 (RISC-V core)
- **Board**: ch32x035g8u6_evt_r0 evaluation board
- **Framework**: noneos-sdk (bare metal, no RTOS)
- **Display**: SSD1306 128x64 OLED via I2C
- **I2C Pins**: PA10 (SCL), PA11 (SDA)
- **I2C Address**: 0x3C (7-bit addressing)

## Code Architecture

### Core Components

- **main.c**: Application entry point with graphics demonstration tests
- **ssd1306.c/.h**: Complete SSD1306 OLED driver with graphics primitives
- **ssd1306_HAL.c/.h**: Hardware abstraction layer for I2C communication
- **ch32x035_it.c/.h**: Interrupt service routines
- **ch32x035_conf.h**: Peripheral library configuration and includes

### SSD1306 Graphics Library

The SSD1306 library provides:
- Basic drawing: pixels, lines, rectangles, circles, ellipses, triangles
- Filled shapes support
- Text rendering with built-in ASCII font
- Double buffering system (buffer1/buffer2 with swap capability)
- Hardware-specific I2C communication abstraction

### Memory Management

- Uses double buffering: `buffer1[]` and `buffer2[]` (1024 bytes each)
- Buffer swapping available via `SSD1306_Buffer_swap()`
- Current active buffer tracked by static pointer

### I2C Communication Pattern

The project uses 7-bit I2C addressing with the CH32X035's I2C peripheral. All display communication goes through the HAL layer, making it portable to other microcontrollers by updating only the HAL implementation.

## Key Functions and Entry Points

- `main()`: src/main.c:89 - System initialization and demo loop
- `SSD1306_Init()`: Initialize display and I2C communication
- `FunctionTest()`: src/main.c:348 - Runs comprehensive graphics primitives testing
- Individual test functions: `PixelTest()`, `LineTest()`, `RectTest()`, `CircleTest()`, `EllipseTest()`, `TriangleTest()`, `StringTest()`, `RoundRectTest()`

## Development Notes

- The project demonstrates proper embedded C structure with HAL abstraction
- All graphics operations require `SSD1306_Update()` to transfer buffer to display
- Serial debugging available at 115200 baud via USART
- I2C pins PA10 (SCL) and PA11 (SDA) connect to SSD1306 display
- Serial debugging available at 115200 baud via USART for development

## Performance Optimizations

This library implements several optimization techniques:
- **Double Buffering**: Eliminates visual tearing and enables smooth animations
- **Hardware Abstraction Layer**: Enables portability across different microcontrollers
- **Optimized Graphics Algorithms**: Bresenham line/circle drawing algorithms
- **UTF-8 Font Support**: ASCII characters and special symbols with 8×8 pixel font

## Testing and Demonstration Functions

- `FunctionTest()`: src/main.c:348 - Comprehensive graphics primitives testing
- `smooth_animation()`: src/main.c:400 - Smooth animation demonstrations  
- `ClockTest()`: src/main.c:425 - Digital and analog clock displays
- Individual test functions: `PixelTest()`, `LineTest()`, `RectTest()`, `CircleTest()`, `EllipseTest()`, `TriangleTest()`, `StringTest()`, `RoundRectTest()`

## Key Configuration Constants

- Display size: 128×64 pixels (SSD1306_WIDTH, SSD1306_HEIGHT)
- Buffer size: 1024 bytes per buffer (double buffering = 2KB total)
- Font: 8×8 pixel ASCII + Greek characters
- I2C frequency: Up to 800kHz Fast Mode