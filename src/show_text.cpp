#include "config.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *dma_display;
extern GFXcanvas16 *canvas;

void showText(const String &text)
{
    const char delim[2] = " ";
    int16_t textX, textY;

    unsigned long startTime = millis();
    unsigned long timeOut = 0;
    //
    char *token;
    int textLen = text.length() + 1;
    char textArray[textLen];
    text.toCharArray(textArray, textLen);
    canvas->setFont();
    canvas->setTextColor(dma_display->color565(255, 255, 255));
    while (1)
    {
        if (millis() - startTime >= timeOut)
        {
            if (timeOut == 0)
            {
                token = strtok(textArray, delim);
            }
            else
            {
                token = strtok(NULL, delim);
            }
            if (token == NULL)
            {
                break;
            }
            //
            uint16_t w, h, r, g, b;
            int16_t x1, y1;
            if (sscanf(token, "(%d,%d,%d)", &r, &g, &b) == 3)
            {
                canvas->setTextColor(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
                continue;
            }
            canvas->getTextBounds(token, 0, 0, &x1, &y1, &w, &h);
            textX = PANEL_RES_X / 2 - w / 2;
            textY = PANEL_RES_Y / 2 - h / 2;
            //
            startTime = millis();
            timeOut = 300 * strlen(token) / 2;
        }
        canvas->fillScreen(0);
        canvas->setTextSize(1);
        canvas->setCursor(textX, textY);
        canvas->print(token);
        //
        dma_display->drawRGBBitmap(0, 0, canvas->getBuffer(), PANEL_RES_X, PANEL_RES_Y);
    }
}