#include "config.h"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>
#include "Fonts/FreeSansBold12pt7b.h"
#include "./fonts/rock_font.h"
#include "./aurora/Playlist.h"

extern MatrixPanel_I2S_DMA *dma_display;
extern GFXcanvas16 *canvas;
extern Playlist *playlist;

void scroll_text(const String &text, uint16_t color)
{

    unsigned long isScrollAnimationDue = 0;
    int delayScrollAnimation = 30;

    int fontSize = 20;

    int textXPosition = PANEL_RES_X * 2;
    int textYPosition = fontSize;

    int16_t xOne, yOne;
    uint16_t w, h;

    canvas->setTextColor(color);
    canvas->setTextSize(1);
    canvas->setTextWrap(false);
    canvas->setFont(&Rockboxcond128pt7b);
    canvas->getTextBounds(text, textXPosition, textYPosition, &xOne, &yOne, &w, &h);

    playlist->loadRandomPattern();

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

        playlist->showFrame();
        canvas->setCursor(textXPosition, textYPosition);
        canvas->print(text);
        dma_display->drawRGBBitmap(0, 0, canvas->getBuffer(), PANEL_RES_X, PANEL_RES_Y);
    }
}
