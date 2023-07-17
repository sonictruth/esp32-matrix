#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>
#include "config.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "./mainfont.h"

extern MatrixPanel_I2S_DMA *dma_display;

CRGB currentColor;
uint16_t time_counter = 0, cycles = 0;
CRGBPalette16 palettes[] = {CloudColors_p, LavaColors_p, OceanColors_p, ForestColors_p, RainbowColors_p};
CRGBPalette16 currentPalette = palettes[0];

void plasmaFrame(MatrixPanel_I2S_DMA *display, GFXcanvas16 *canvas)
{
    for (int x = 0; x < PANEL_RES_X; x++)
    {
        for (int y = 0; y < PANEL_RES_Y; y++)
        {
            int16_t v = 0;
            uint8_t wibble = sin8(time_counter);
            v += sin16(x * wibble * 3 + time_counter);
            v += cos16(y * (128 - wibble) + time_counter);
            v += sin16(y * x * cos8(-time_counter) / 8);

            currentColor = ColorFromPalette(currentPalette, (v >> 8) + 127, 80); //, brightness, currentBlendType);
            uint16_t color = display->color565(currentColor.r, currentColor.g, currentColor.b);
            canvas->drawPixel(x, y, color);
        }
    }
    ++time_counter;
    ++cycles;

    if (cycles >= 1024)
    {
        time_counter = 0;
        cycles = 0;
        currentPalette = palettes[random(0, sizeof(palettes) / sizeof(palettes[0]))];
    }
}

void scrollText(String text, uint16_t color)
{
    GFXcanvas16 canvas(PANEL_RES_X, PANEL_RES_Y);
    int16_t xOne, yOne;
    uint16_t w, h;
    unsigned long isAnimationDue = 0;
    int delayBetweeenAnimations = 60;
    int fontSize = 26;
    int textXPosition = PANEL_RES_X;
    int textYPosition = fontSize;

    canvas.setTextColor(color);
    canvas.setTextSize(1);
    canvas.setTextWrap(false);
    canvas.setFont(&ALTERNATIVE_ROCK_Border_40017pt7b);
    canvas.getTextBounds(text, textXPosition, textYPosition, &xOne, &yOne, &w, &h);
    while (1)
    {
        unsigned long now = millis();
        if (now > isAnimationDue)
        {
            isAnimationDue = now + delayBetweeenAnimations;
            textXPosition -= 1;
            if (textXPosition + w <= 0)
            {
                dma_display->clearScreen();
                break;
            }
        }
        plasmaFrame(dma_display, &canvas);
        canvas.setTextColor(dma_display->color565(255, 255, 255));
        canvas.setCursor(textXPosition, textYPosition);
        canvas.print(text);

        // canvas.setTextColor(color);
        // canvas.setCursor(textXPosition - 1, textYPosition - 1);
        // canvas.print(text);

        dma_display->drawRGBBitmap(0, 0, canvas.getBuffer(), PANEL_RES_X, PANEL_RES_Y);
    }
}