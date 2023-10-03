#include "config.h"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>
#include "Fonts/FreeSansBold12pt7b.h"
#include "./fonts/rock_font.h"
#include "./aurora/effects.h"
#include "./aurora/PatternPlasma.h"
#include "./aurora/PatternSnake.h"
#include "./aurora/PatternInfinity.h"

extern MatrixPanel_I2S_DMA *dma_display;
extern GFXcanvas16 *canvas;

void scroll_text(const String &text, uint16_t color)
{

    unsigned long isScrollAnimationDue = 0;
    int delayScrollAnimation = 30;

    unsigned long isBackgroundAnimationDue = 0;
    int delayBackgroundAnimation = 40;

    int fontSize = 20;

    int textXPosition = PANEL_RES_X * 2;
    int textYPosition = fontSize;

    int16_t xOne, yOne;
    uint16_t w, h;

    dma_display->clearScreen();

    canvas->setTextColor(color);
    canvas->setTextSize(1);
    canvas->setTextWrap(false);
    canvas->setFont(&Rockboxcond128pt7b);
    canvas->getTextBounds(text, textXPosition, textYPosition, &xOne, &yOne, &w, &h);

    PatternSnake ps;
    effects->ClearFrame();
    effects->loadPalette(random(0, 8));

    while (1)
    {
        unsigned long now = millis();
        if (now > isScrollAnimationDue)
        {
            isScrollAnimationDue = now + delayScrollAnimation;
            textXPosition -= 1;
            if (textXPosition + w <= -PANEL_RES_X)
            {
                dma_display->clearScreen();
                break;
            }
        }
        if (now > isBackgroundAnimationDue)
        {

            isBackgroundAnimationDue = now + delayBackgroundAnimation;
            ps.drawFrame();
        }

        effects->ShowFrame();
        canvas->setCursor(textXPosition, textYPosition);
        canvas->print(text);
        dma_display->drawRGBBitmap(0, 0, canvas->getBuffer(), PANEL_RES_X, PANEL_RES_Y);
    }
}
