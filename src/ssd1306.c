#include "debug.h"
#include <string.h>
#include <stdint.h>

#include "ssd1306.h"
#include "ssd1306_HAL.h"
#include "ssd1306_font.h"

static uint8_t buffer1[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
static uint8_t buffer2[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static uint8_t *buffer = buffer1;         // Current drawing buffer
static uint8_t *display_buffer = buffer2; // Last displayed buffer

// Dirty region tracking for partial updates
static uint8_t dirty_pages = 0xFF; // Bitmask of dirty pages (8 pages for 64px height)
static uint8_t dirty_left = 0, dirty_right = SSD1306_WIDTH - 1;
static uint8_t force_full_update = 1;

void ssdtest1(void)
{
    memcpy(buffer, ascii_font, SSD1306_BUFFER_SIZE);
}

// void ssdtest2(void) {
//     memcpy(buffer, ascii_font, sizeof(ascii_font)-SSD1306_BUFFER_SIZE);
// }

void SSD1306_Buffer_swap(void)
{
    // Swap drawing and display buffers
    uint8_t *temp = buffer;
    buffer = display_buffer;
    display_buffer = temp;

    // Reset dirty tracking
    dirty_pages = 0;
    dirty_left = SSD1306_WIDTH;
    dirty_right = 0;
    force_full_update = 0;
}

// Mark region as dirty for partial updates
static void mark_dirty_region(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    if (force_full_update)
        return;

    // Bounds checking
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        return;
    if (w == 0 || h == 0)
        return;

    // Clamp to display bounds
    if (x + w > SSD1306_WIDTH)
        w = SSD1306_WIDTH - x;
    if (y + h > SSD1306_HEIGHT)
        h = SSD1306_HEIGHT - y;

    uint8_t page_start = y >> 3;
    uint8_t page_end = (y + h - 1) >> 3;
    uint8_t x_end = x + w - 1;

    // Update dirty page mask
    for (uint8_t p = page_start; p <= page_end && p < 8; p++)
    {
        dirty_pages |= (1 << p);
    }

    // Update dirty column range
    if (dirty_left > dirty_right)
    {
        // First dirty region - initialize range
        dirty_left = x;
        dirty_right = x_end;
    }
    else
    {
        // Expand existing range
        if (x < dirty_left)
            dirty_left = x;
        if (x_end > dirty_right)
            dirty_right = x_end;
    }
}

void SSD1306_Init(void)
{
    uint8_t tx_buffer[14];

    SSD1306_IIC_Init_HAL();

    // Initialize the SSD1306 OLED display

    tx_buffer[0] = SSD1306_CMD_SET_DISPLAY_OFF;                     // Turn off the display
    tx_buffer[1] = SSD1306_CMD_SET_COM_PINS_HARDWARE_CONFIGURATION; // Set COM pins hardware configuration
    if (SSD1306_HEIGHT == 64)
    {
        tx_buffer[2] = 0x12; // Default COM pin configuration for 64 lines display
    }
    else
    {
        tx_buffer[2] = 0x02; // Default COM pin configuration for 32 lines display
    } // Default COM pin configuration
    tx_buffer[3] = SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL; // Set VCOMH deselect level
    tx_buffer[4] = 0x50;                                 // 0.90*VCC
    tx_buffer[5] = SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO;
    tx_buffer[6] = 0xF0;
    tx_buffer[7] = SSD1306_CMD_SET_CHARGE_PUMP;      // Set charge pump
    tx_buffer[8] = 0x14;                             // Enable charge pump
    tx_buffer[9] = SSD1306_CMD_SET_PRECHARGE_PERIOD; // Set pre-charge period
    tx_buffer[10] = 0xFF;                            // Pre-charge period
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 11);

    tx_buffer[0] = SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE;     // Set memory addressing mode
    tx_buffer[1] = 0x00;                                       // Horizontal addressing mode
    tx_buffer[2] = SSD1306_CMD_SET_SEGMENT_REMAP_INVERSE;      // Set segment re-map
    tx_buffer[3] = SSD1306_CMD_SET_COM_SCAN_DIRECTION_INVERSE; // Set COM scan direction normal

    tx_buffer[4] = SSD1306_CMD_SET_DISPLAY_OFFSET; // Set display offset
    tx_buffer[5] = 0x00;
    tx_buffer[6] = SSD1306_CMD_SET_LOWER_COLUMN_ADDRESS_OFFSET + 0x00;  // Set higher column address offset
    tx_buffer[7] = SSD1306_CMD_SET_HIGHER_COLUMN_ADDRESS_OFFSET + 0x00; // Set higher column address offset
    tx_buffer[8] = SSD1306_CMD_SET_DISPLAY_START_LINE_OFFSET + 0x00;    // Set display start line offset
    tx_buffer[9] = SSD1306_CMD_SET_MULTIPLEX_RATIO;
    tx_buffer[10] = SSD1306_HEIGHT - 1; // Set MUX ratio to 32 (for 64 lines display)

    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 11);

    Delay_Ms(100); // Wait for the display to initialize

    tx_buffer[0] = SSD1306_CMD_SET_CONTRAST;           // Set contrast control
    tx_buffer[1] = 0xCF;                               // Contrast value
    tx_buffer[2] = SSD1306_CMD_SET_DISPLAY_ALL_NORMAL; // Set display to normal mode
    tx_buffer[3] = SSD1306_CMD_SET_NORMAL_DISPLAY;     // Set normal display
    tx_buffer[4] = SSD1306_CMD_SET_SCROLL_STOP;        // Stop any scrolling
    tx_buffer[5] = SSD1306_CMD_SET_DISPLAY_ON;         // Turn on the display
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 6);

    // Initialize buffers and dirty tracking
    SSD1306_Clear();
    force_full_update = 1; // Force first update to be full
}

void SSD1306_Clear(void)
{
    memset(buffer, 0, SSD1306_BUFFER_SIZE);
    // Mark entire display as dirty
    dirty_pages = 0xFF;
    dirty_left = 0;
    dirty_right = SSD1306_WIDTH - 1;
}

void SSD1306_Update(void)
{
    if (dirty_pages == 0 && !force_full_update)
    {
        return; // Nothing to update
    }

    uint8_t cmd_buffer[4];
    uint8_t update_left, update_right;

    if (force_full_update)
    {
        // Full screen update
        update_left = 0;
        update_right = SSD1306_WIDTH - 1;
    }
    else
    {
        // Partial update - check if we have valid dirty region
        if (dirty_left > dirty_right)
        {
            // No valid dirty region, return
            dirty_pages = 0;
            return;
        }
        update_left = dirty_left;
        update_right = dirty_right;
    }

    // Set column address range for burst transfer
    cmd_buffer[0] = SSD1306_CMD_SET_COLUMN_ADDRESS;
    cmd_buffer[1] = update_left;
    cmd_buffer[2] = update_right;
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, cmd_buffer, 3);

    // Process pages that need updating
    uint8_t pages_to_update = force_full_update ? 0xFF : dirty_pages;

    for (uint8_t page = 0; page < 8; page++)
    {
        if (!(pages_to_update & (1 << page)))
            continue;

        // Set page address
        cmd_buffer[0] = SSD1306_CMD_SET_PAGE_ADDRESS;
        cmd_buffer[1] = page;
        cmd_buffer[2] = page;
        SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, cmd_buffer, 3);

        // Send entire row in one burst transfer
        uint16_t start_idx = page * SSD1306_WIDTH + update_left;
        uint8_t width = update_right - update_left + 1;
        SSD1306_IIC_HAL(SSD1306_MODE_DATA, &buffer[start_idx], width);

        // Copy updated region to display buffer for differential comparison
        memcpy(&display_buffer[start_idx], &buffer[start_idx], width);
    }

    // Reset dirty tracking
    dirty_pages = 0;
    dirty_left = SSD1306_WIDTH;
    dirty_right = 0;
    force_full_update = 0;
}

// Fast full screen update (for initialization)
void SSD1306_Update_Full(void)
{
    uint8_t cmd_buffer[3];

    // Set full screen address range
    cmd_buffer[0] = SSD1306_CMD_SET_COLUMN_ADDRESS;
    cmd_buffer[1] = 0;
    cmd_buffer[2] = SSD1306_WIDTH - 1;
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, cmd_buffer, 3);

    cmd_buffer[0] = SSD1306_CMD_SET_PAGE_ADDRESS;
    cmd_buffer[1] = 0;
    cmd_buffer[2] = (SSD1306_HEIGHT / 8) - 1;
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, cmd_buffer, 3);

    // Send entire buffer in one burst
    SSD1306_IIC_HAL(SSD1306_MODE_DATA, buffer, SSD1306_BUFFER_SIZE);

    // Copy to display buffer
    memcpy(display_buffer, buffer, SSD1306_BUFFER_SIZE);

    // Reset dirty tracking
    dirty_pages = 0;
    dirty_left = SSD1306_WIDTH;
    dirty_right = 0;
    force_full_update = 0;
}

void SSD1306_DisplayOn(void)
{
    uint8_t tx_buffer[1];
    tx_buffer[0] = SSD1306_CMD_SET_DISPLAY_ON; // Set display to normal mode
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 1);
}

void SSD1306_DisplayOff(void)
{
    uint8_t tx_buffer[1];
    tx_buffer[0] = SSD1306_CMD_SET_DISPLAY_OFF; // Turn off the display
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 1);
}

void SSD1306_DisplayAllOn(void)
{
    uint8_t tx_buffer[1];
    tx_buffer[0] = SSD1306_CMD_SET_DISPLAY_ALL_ON;
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 1);
}

void SSD1306_DisplayNormal(void)
{
    uint8_t tx_buffer[2];
    tx_buffer[0] = SSD1306_CMD_SET_NORMAL_DISPLAY;
    tx_buffer[1] = SSD1306_CMD_SET_DISPLAY_ALL_NORMAL;
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 2);
}

void SSD1306_DisplayInverse(void)
{
    uint8_t tx_buffer[1];
    tx_buffer[0] = SSD1306_CMD_SET_INVERSE_DISPLAY; // Set inverse display
    SSD1306_IIC_HAL(SSD1306_MODE_COMMAND, tx_buffer, 1);
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        return; // Out of bounds
    }

    if (color)
    {
        buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8)); // Set pixel
    }
    else
    {
        buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8)); // Clear pixel
    }

    // Mark this region as dirty
    mark_dirty_region(x, y, 1, 1);
}

void SSD1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
    // Mark dirty region once for entire line
    uint8_t min_x = (x0 < x1) ? x0 : x1;
    uint8_t max_x = (x0 > x1) ? x0 : x1;
    uint8_t min_y = (y0 < y1) ? y0 : y1;
    uint8_t max_y = (y0 > y1) ? y0 : y1;
    mark_dirty_region(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1);

    // Fast Bresenham's algorithm with inline pixel drawing
    int dx = (int)x1 - (int)x0;
    int dy = (int)y1 - (int)y0;
    int sx = (dx >= 0) ? 1 : -1;
    int sy = (dy >= 0) ? 1 : -1;
    dx = (dx >= 0) ? dx : -dx;
    dy = (dy >= 0) ? dy : -dy;

    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1)
    {
        // Inline pixel drawing - don't call mark_dirty_region for each pixel
        if (x0 < SSD1306_WIDTH && y0 < SSD1306_HEIGHT)
        {
            if (color)
            {
                buffer[x0 + (y0 / 8) * SSD1306_WIDTH] |= (1 << (y0 % 8));
            }
            else
            {
                buffer[x0 + (y0 / 8) * SSD1306_WIDTH] &= ~(1 << (y0 % 8));
            }
        }

        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    // Draw the four sides of the rectangle
    SSD1306_DrawLine(x, y, x + width - 1, y, color);                           // Top side
    SSD1306_DrawLine(x + width - 1, y, x + width - 1, y + height - 1, color);  // Right side
    SSD1306_DrawLine(x + width - 1, y + height - 1, x, y + height - 1, color); // Bottom side
    SSD1306_DrawLine(x, y + height - 1, x, y, color);                          // Left side
}

void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    // Fill the rectangle by drawing horizontal lines
    for (uint8_t i = 0; i < height; i++)
    {
        SSD1306_DrawLine(x, y + i, x + width - 1, y + i, color);
    }
}

void SSD1306_DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color)
{
    int x = radius;
    int y = 0;
    int err = 1 - radius;

    while (x >= y)
    {
        SSD1306_DrawPixel(x0 + x, y0 + y, color);
        SSD1306_DrawPixel(x0 + y, y0 + x, color);
        SSD1306_DrawPixel(x0 - y, y0 + x, color);
        SSD1306_DrawPixel(x0 - x, y0 + y, color);
        SSD1306_DrawPixel(x0 - x, y0 - y, color);
        SSD1306_DrawPixel(x0 - y, y0 - x, color);
        SSD1306_DrawPixel(x0 + y, y0 - x, color);
        SSD1306_DrawPixel(x0 + x, y0 - y, color);

        y++;
        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void SSD1306_FillCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (y >= x)
    {
        // Draw horizontal lines between symmetric points
        for (int i = x0 - x; i <= x0 + x; i++)
        {
            SSD1306_DrawPixel(i, y0 + y, color);
            SSD1306_DrawPixel(i, y0 - y, color);
        }
        for (int i = x0 - y; i <= x0 + y; i++)
        {
            SSD1306_DrawPixel(i, y0 + x, color);
            SSD1306_DrawPixel(i, y0 - x, color);
        }
        x++;
        if (d < 0)
        {
            d += 2 * x + 1;
        }
        else
        {
            y--;
            d += 2 * (x - y) + 1;
        }
    }
}

void SSD1306_DrawEllipse(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
    // Fast parameter setup
    int xc = x0 + (x1 >> 1);
    int yc = y0 + (y1 >> 1);
    int a = x1 >> 1;
    int b = y1 >> 1;

    if (a == 0 && b == 0)
    {
        SSD1306_DrawPixel(xc, yc, color);
        return;
    }
    if (a == 0)
    {
        SSD1306_DrawLine(xc, y0, xc, y0 + y1 - 1, color);
        return;
    }
    if (b == 0)
    {
        SSD1306_DrawLine(x0, yc, x0 + x1 - 1, yc, color);
        return;
    }

    // Optimized ellipse algorithm
    register int x = 0, y = b;
    register int a2 = a * a, b2 = b * b;
    register int dx = 0, dy = (a2 << 1) * y;
    register int err = b2 - a2 * b + (a2 >> 2);
    register int xc_px, xc_mx, yc_py, yc_my;

    // Region 1
    do
    {
        xc_px = xc + x;
        xc_mx = xc - x;
        yc_py = yc + y;
        yc_my = yc - y;

        SSD1306_DrawPixel(xc_px, yc_py, color);
        SSD1306_DrawPixel(xc_mx, yc_py, color);
        SSD1306_DrawPixel(xc_px, yc_my, color);
        SSD1306_DrawPixel(xc_mx, yc_my, color);

        if (err < 0)
        {
            x++;
            dx += (b2 << 1);
            err += dx + b2;
        }
        else
        {
            x++;
            y--;
            dx += (b2 << 1);
            dy -= (a2 << 1);
            err += dx - dy + b2;
        }
    } while (dx < dy);

    // Region 2
    err = b2 * (x + 1) * (x + 1) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0)
    {
        xc_px = xc + x;
        xc_mx = xc - x;
        yc_py = yc + y;
        yc_my = yc - y;

        SSD1306_DrawPixel(xc_px, yc_py, color);
        SSD1306_DrawPixel(xc_mx, yc_py, color);
        SSD1306_DrawPixel(xc_px, yc_my, color);
        SSD1306_DrawPixel(xc_mx, yc_my, color);

        if (err > 0)
        {
            y--;
            dy -= (a2 << 1);
            err -= dy + a2;
        }
        else
        {
            x++;
            y--;
            dx += (b2 << 1);
            dy -= (a2 << 1);
            err += dx - dy + a2;
        }
    }
}

void SSD1306_FillEllipse(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
    // Fast parameter setup
    register int xc = x0 + (x1 >> 1);
    register int yc = y0 + (y1 >> 1);
    register int a = x1 >> 1;
    register int b = y1 >> 1;

    if (a == 0 || b == 0)
        return;

    // Optimized fill using integer arithmetic
    register int a2 = a * a, b2 = b * b;
    register int y, dy, dx;
    register long dy2, dx_squared;

    for (y = y0; y < y0 + y1; y++)
    {
        dy = y - yc;
        dy2 = (long)dy * dy;

        if (dy2 <= b2)
        {
            dx_squared = (long)a2 * (b2 - dy2) / b2;

            // Fast integer square root approximation
            dx = 0;
            if (dx_squared > 0)
            {
                register long test = 1;
                while (test <= dx_squared)
                {
                    dx++;
                    test = (long)dx * dx;
                }
                dx--;
            }

            if (dx > 0)
            {
                SSD1306_DrawLine(xc - dx, y, xc + dx, y, color);
            }
        }
    }
}

void SSD1306_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    // Draw the three sides of the triangle
    SSD1306_DrawLine(x0, y0, x1, y1, color); // Side 1
    SSD1306_DrawLine(x1, y1, x2, y2, color); // Side 2
    SSD1306_DrawLine(x2, y2, x0, y0, color); // Side 3
}

void SSD1306_FillTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    // Fast vertex sorting
    register int tx, ty;
    if (y0 > y1)
    {
        tx = x0;
        x0 = x1;
        x1 = tx;
        ty = y0;
        y0 = y1;
        y1 = ty;
    }
    if (y1 > y2)
    {
        tx = x1;
        x1 = x2;
        x2 = tx;
        ty = y1;
        y1 = y2;
        y2 = ty;
    }
    if (y0 > y1)
    {
        tx = x0;
        x0 = x1;
        x1 = tx;
        ty = y0;
        y0 = y1;
        y1 = ty;
    }

    // Precompute deltas to avoid division in loop
    register int dy01 = y1 - y0, dy02 = y2 - y0, dy12 = y2 - y1;
    register int dx01 = x1 - x0, dx02 = x2 - x0, dx12 = x2 - x1;

    if (dy02 == 0)
        return; // Degenerate triangle

    // Fill triangle with optimized scanline
    register int y, xa, xb;
    for (y = y0; y <= y2; y++)
    {
        if (y <= y1 && dy01 != 0)
        {
            xa = x0 + dx01 * (y - y0) / dy01;
        }
        else if (dy12 != 0)
        {
            xa = x1 + dx12 * (y - y1) / dy12;
        }
        else
        {
            xa = x1;
        }

        xb = x0 + dx02 * (y - y0) / dy02;

        if (xa > xb)
        {
            tx = xa;
            xa = xb;
            xb = tx;
        }
        if (xa != xb)
            SSD1306_DrawLine(xa, y, xb, y, color);
    }
}

void SSD1306_DrawRoundRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t radius, uint8_t color)
{
    // Fast parameter calculation
    register int right = x0 + x1 - 1, bottom = y0 + y1 - 1;
    register int max_r = (x1 < y1) ? (x1 >> 1) : (y1 >> 1);
    if (radius > max_r)
        radius = max_r;

    // Draw sides in one go
    register int r = radius;
    SSD1306_DrawLine(x0 + r, y0, right - r, y0, color);
    SSD1306_DrawLine(right, y0 + r, right, bottom - r, color);
    SSD1306_DrawLine(x0 + r, bottom, right - r, bottom, color);
    SSD1306_DrawLine(x0, y0 + r, x0, bottom - r, color);

    // Optimized corner drawing
    register int x = 0, y = r, d = 3 - (r << 1);
    register int cx1 = x0 + r, cy1 = y0 + r;
    register int cx2 = right - r, cy2 = bottom - r;

    while (y >= x)
    {
        // Draw all 8 corner pixels at once
        SSD1306_DrawPixel(cx1 - x, cy1 - y, color);
        SSD1306_DrawPixel(cx1 - y, cy1 - x, color);
        SSD1306_DrawPixel(cx2 + x, cy1 - y, color);
        SSD1306_DrawPixel(cx2 + y, cy1 - x, color);
        SSD1306_DrawPixel(cx2 + x, cy2 + y, color);
        SSD1306_DrawPixel(cx2 + y, cy2 + x, color);
        SSD1306_DrawPixel(cx1 - x, cy2 + y, color);
        SSD1306_DrawPixel(cx1 - y, cy2 + x, color);

        if (d < 0)
        {
            d += (x << 2) + 6;
            x++;
        }
        else
        {
            d += ((x - y) << 2) + 10;
            x++;
            y--;
        }
    }
}

void SSD1306_FillRoundRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t radius, uint8_t color)
{
    // Fast parameter setup
    register int right = x0 + x1 - 1, bottom = y0 + y1 - 1;
    register int max_r = (x1 < y1) ? (x1 >> 1) : (y1 >> 1);
    if (radius > max_r)
        radius = max_r;

    register int r = radius;

    // Fill main areas quickly
    SSD1306_FillRect(x0, y0 + r, x1, y1 - (r << 1), color);
    SSD1306_FillRect(x0 + r, y0, x1 - (r << 1), r, color);
    SSD1306_FillRect(x0 + r, bottom - r + 1, x1 - (r << 1), r, color);

    // Optimized corner filling
    register int x = 0, y = r, d = 3 - (r << 1);
    register int cx1 = x0 + r, cy1 = y0 + r;
    register int cx2 = right - r, cy2 = bottom - r;

    while (y >= x)
    {
        // Fill corner areas with horizontal lines
        SSD1306_DrawLine(cx1 - x, cy1 - y, cx1 + x, cy1 - y, color);
        SSD1306_DrawLine(cx1 - x, cy2 + y, cx1 + x, cy2 + y, color);
        SSD1306_DrawLine(cx2 - x, cy1 - y, cx2 + x, cy1 - y, color);
        SSD1306_DrawLine(cx2 - x, cy2 + y, cx2 + x, cy2 + y, color);

        if (y != x)
        {
            SSD1306_DrawLine(cx1 - y, cy1 - x, cx1 + y, cy1 - x, color);
            SSD1306_DrawLine(cx1 - y, cy2 + x, cx1 + y, cy2 + x, color);
            SSD1306_DrawLine(cx2 - y, cy1 - x, cx2 + y, cy1 - x, color);
            SSD1306_DrawLine(cx2 - y, cy2 + x, cx2 + y, cy2 + x, color);
        }

        if (d < 0)
        {
            d += (x << 2) + 6;
            x++;
        }
        else
        {
            d += ((x - y) << 2) + 10;
            x++;
            y--;
        }
    }
}

// Helper function to lookup UTF-8 character
static int find_utf8_char(const uint8_t *utf8_bytes)
{
    for (int i = 0; i < sizeof(utf8_lookup) / sizeof(utf8_lookup[0]); i++)
    {
        if (utf8_lookup[i].utf8_bytes[0] == utf8_bytes[0] &&
            utf8_lookup[i].utf8_bytes[1] == utf8_bytes[1] &&
            (utf8_lookup[i].utf8_bytes[2] == 0 || utf8_lookup[i].utf8_bytes[2] == utf8_bytes[2]))
        {
            return utf8_lookup[i].font_index;
        }
    }
    return -1; // Not found
}

void SSD1306_DrawChar(uint8_t x, uint8_t y, char c, uint8_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        return; // Out of bounds
    }

    if (c < 32 || c > 126)
    {
        return; // Unsupported character
    }

    const uint8_t *font = ascii_font[c - 32]; // Get the font data for the character

    // Mark dirty region for character
    mark_dirty_region(x, y, 8, 8);

    for (uint8_t i = 0; i < 8; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            if (font[i] & (1 << j))
            {
                // Direct pixel drawing to avoid multiple dirty region marks
                if (x + i < SSD1306_WIDTH && y + j < SSD1306_HEIGHT)
                {
                    if (color)
                    {
                        buffer[(x + i) + ((y + j) / 8) * SSD1306_WIDTH] |= (1 << ((y + j) % 8));
                    }
                    else
                    {
                        buffer[(x + i) + ((y + j) / 8) * SSD1306_WIDTH] &= ~(1 << ((y + j) % 8));
                    }
                }
            }
        }
    }
}

// Draw UTF-8 character (Greek letters, special symbols)
void SSD1306_DrawCharUTF8(uint8_t x, uint8_t y, const uint8_t *utf8_bytes, uint8_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        return; // Out of bounds
    }

    int font_index = find_utf8_char(utf8_bytes);
    if (font_index < 0)
    {
        return; // Character not found
    }

    const uint8_t *font = extended_font[font_index];

    // Mark dirty region for character
    mark_dirty_region(x, y, 8, 8);

    for (uint8_t i = 0; i < 8; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            if (font[i] & (1 << j))
            {
                // Direct pixel drawing to avoid multiple dirty region marks
                if (x + i < SSD1306_WIDTH && y + j < SSD1306_HEIGHT)
                {
                    if (color)
                    {
                        buffer[(x + i) + ((y + j) / 8) * SSD1306_WIDTH] |= (1 << ((y + j) % 8));
                    }
                    else
                    {
                        buffer[(x + i) + ((y + j) / 8) * SSD1306_WIDTH] &= ~(1 << ((y + j) % 8));
                    }
                }
            }
        }
    }
}

void SSD1306_DrawString(uint8_t x, uint8_t y, const char *str, uint8_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        return; // Out of bounds
    }

    const uint8_t *utf8_str = (const uint8_t *)str;

    while (*utf8_str != 0) // Explicit check for null terminator
    {
        if (x >= SSD1306_WIDTH - 7) // Check for tight fit (8 pixels wide)
        {
            x = 0; // Move to next line if out of bounds
            y += 8;
            if (y >= SSD1306_HEIGHT - 7) // Check for tight vertical fit
            {
                break; // Stop if out of vertical bounds
            }
        }

        // Check for UTF-8 multi-byte character
        if (*utf8_str >= 0x80)
        {
            // Multi-byte UTF-8 character
            uint8_t utf8_bytes[3] = {0};
            int byte_count = 0;

            // Determine number of bytes in UTF-8 character
            if ((*utf8_str & 0xE0) == 0xC0)
                byte_count = 2; // 110xxxxx
            else if ((*utf8_str & 0xF0) == 0xE0)
                byte_count = 3; // 1110xxxx
            else
                byte_count = 1; // Invalid, treat as single byte

            // Copy UTF-8 bytes and ensure we don't go past string end
            for (int i = 0; i < byte_count && i < 3 && utf8_str[i] != 0; i++)
            {
                utf8_bytes[i] = utf8_str[i];
            }

            SSD1306_DrawCharUTF8(x, y, utf8_bytes, color);
            utf8_str += byte_count;
        }
        else if (*utf8_str >= 32 && *utf8_str <= 126)
        {
            // Regular ASCII character (only printable characters)
            SSD1306_DrawChar(x, y, *utf8_str, color);
            utf8_str++;
        }
        else
        {
            // Skip invalid characters (including null terminator, control chars)
            utf8_str++;
            continue; // Don't advance x position for invalid characters
        }

        x += 8; // Move to next character position (no gap)
    }
}