#include "config.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>
#include "Fonts/FreeSansBold12pt7b.h"
#include "./mainfont.h"
#include "./aurora/effects.h"
#include "./aurora/PatternPlasma.h"
#include "./aurora/PatternSnake.h"
#include "./aurora/PatternInfinity.h"

extern MatrixPanel_I2S_DMA *dma_display;
Effects *effects;
GFXcanvas16 *canvas;

void scrollText(const String& text, uint16_t color)
{
    canvas = new GFXcanvas16(PANEL_RES_X, PANEL_RES_Y);
    effects = new Effects();
    effects->Setup();
    int16_t xOne, yOne;
    uint16_t w, h;

    PatternInfinity ps;

    unsigned long isAnimationDue = 0;
    int delayBetweeenAnimations = 30;
    int fontSize = 20;
    int textXPosition = PANEL_RES_X;
    int textYPosition = fontSize;

    canvas->setTextColor(color);
    canvas->setTextSize(1);
    canvas->setTextWrap(false);
    canvas->setFont(&Creepster_Caps_Regular_25);
    canvas->getTextBounds(text, textXPosition, textYPosition, &xOne, &yOne, &w, &h);
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
        ps.drawFrame();
        canvas->setCursor(textXPosition, textYPosition);
        canvas->print(text);
        dma_display->drawRGBBitmap(0, 0, canvas->getBuffer(), PANEL_RES_X, PANEL_RES_Y);
    }
    delete canvas;
    delete effects;
}