#include "config.h"
#include "./fonts/chicago_font.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "./aurora/Playlist.h"

extern MatrixPanel_I2S_DMA *dma_display;
extern GFXcanvas16 *canvas;
extern Playlist *playlist;

void show_text(const String &text)
{

    const char delim[2] = " ";
    int16_t textX, textY;
    int8_t fontWeight = 5;
    int8_t fontHeight = 10;
    uint16_t w, h, r, g, b;
    int16_t x1, y1;
    unsigned long startTime = millis();
    unsigned long timeOut = 0;
    bool isFirstTime = true;

    char *token;
    int textLen = text.length() + 1;
    char textArray[textLen];
    text.toCharArray(textArray, textLen);

    bool isScrollingText = false;

    canvas->setTextColor(getRandomColor());
    canvas->setFont(&Chicago);
    canvas->setTextWrap(false);
    canvas->setTextSize(1);

    unsigned long isAnimationDue = 0;
    int delayBetweeenScrollPaint = 190;

    playlist->loadRandomPattern();
    
    while (1)
    {
        unsigned long now = millis();
        if (now - startTime >= timeOut || isFirstTime)
        {
            if (isFirstTime) 
            {
                token = strtok(textArray, delim);
                isFirstTime = false;
            }
            else
            {
                token = strtok(NULL, delim);
            }
            if (token == NULL)
            {
                dma_display->clearScreen();
                break; // No more tokens exit
            }
            if (sscanf(token, "(%d,%d,%d)", &r, &g, &b) == 3)
            {
                canvas->setTextColor(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
                continue; // Color instruction
            }
            canvas->getTextBounds(token, 0, 0, &x1, &y1, &w, &h);

            isScrollingText = w > PANEL_RES_X;

            textY = (PANEL_RES_Y / 2 + fontHeight / 2);

            if (isScrollingText)
            {
                textX = fontWeight;
                timeOut = 100000;
            }
            else
            {
                textX = (PANEL_RES_X / 2 - w / 2);
                timeOut = 250 + (strlen(token) * 150);
            }

            startTime = millis();
        }

        playlist->showFrame();
        if (strcmp(token, "...") != 0) // Do not print ...
        {
            if (isScrollingText && now > isAnimationDue)
            {
                isAnimationDue = now + delayBetweeenScrollPaint;
                textX -= 1;
                if (textX < PANEL_RES_X - w - fontWeight)
                {
                    timeOut = 0;
                }
            }
            canvas->setCursor(textX, textY);
            canvas->print(token);
        }
        dma_display->drawRGBBitmap(0, 0, canvas->getBuffer(), PANEL_RES_X, PANEL_RES_Y);
    }
}
