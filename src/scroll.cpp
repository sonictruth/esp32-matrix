#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "config.h"


extern MatrixPanel_I2S_DMA *dma_display;
extern uint16_t myBLACK;
int16_t xOne, yOne;
uint16_t w, h;

void scrollText()
{
    String text = "Hello testing 12345678 ESP32 World!";
    unsigned long isAnimationDue;
    int delayBetweeenAnimations = 25;    // Smaller == faster1
    int textXPosition = PANEL_RES_X * 1; // Will start off screen
    int textYPosition = PANEL_RES_Y / 2 - 8;
    dma_display->clearScreen();
    while (textXPosition > -300)
    {
        Serial.println(textXPosition);
        unsigned long now = millis();
        if (now > isAnimationDue)
        {
            isAnimationDue = now + delayBetweeenAnimations;

            textXPosition -= 1;

            // Checking is the very right of the text off screen to the left
            dma_display->getTextBounds(text, textXPosition, textYPosition, &xOne, &yOne, &w, &h);
            if (textXPosition + w <= 0)
            {
                textXPosition = PANEL_RES_X;
            }

            dma_display->setCursor(textXPosition, textYPosition);
            dma_display->fillRect(0, textYPosition, dma_display->width(), 16, myBLACK);

            dma_display->print(text);
#ifdef ENABLE_DOUBLE_BUFFER
            dma_display->flipDMABuffer();
#endif
        }
    }
}