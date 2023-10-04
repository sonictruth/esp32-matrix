#ifndef Playlist_H
#define Playlist_H
/* Patterns */

#include "Effects.h"

#include "PatternSnake.h"
#include "PatternInfinity.h"
#include "PatternFlowField.h"
#include "PatternWave.h"
#include "PatternElectricMandala.h"
#include "PatternSpiral.h"
#include "PatternLife.h"
#include "PatternMaze.h"
#include "PatternFlock.h"
#include "PatternRadar.h"
#include "PatternSwirl.h"
#include "PatternIncrementalDrift.h"
#include "PatternMunch.h"
#include "PatternSimplexNoise.h"

Effects *effects = nullptr;

class Playlist
{
private:
    PatternSnake patternSnake;
    PatternFlowField patternFlowField;
    PatternElectricMandala patternElectricMandala;

    PatternLife patternLife;
    PatternFlock patternFlock;
    PatternSwirl patternSwirl;
    PatternIncrementalDrift patternIncrementalDrift;
    // PatternMunch patternMunch;
    // PatternSimplexNoise patternSimplexNoise;
    // PatternWave patternWave;
    // PatternSpiral patternSpiral;

    const static int PATTERN_COUNT = 7;
    Drawable *items[PATTERN_COUNT] = {
        &patternLife,
        &patternElectricMandala,
        &patternIncrementalDrift,
        &patternSnake,
        &patternFlowField,
        &patternFlock,
        &patternSwirl,
    };

    int currentIndex = 0;
    Drawable *currentItem = items[currentIndex];
    unsigned long isRenderingDue = 0;
    int renderingDelay = 40;

    void renderFrame()
    {
        /*
        EVERY_N_SECONDS(10)
        {
            loadPattern();
        }
        */
        renderingDelay = currentItem->drawFrame();
    }

public:
    void loadRandomPattern()
    {
        effects->ClearFrame();
        effects->RandomPalette();
        currentItem = items[random(0, PATTERN_COUNT)];
        currentItem->start();
    }
    void loadNextPattern()
    {
        effects->ClearFrame();
        effects->RandomPalette();
        currentItem = items[currentIndex];
        currentItem->start();
        currentIndex++;
        if (currentIndex >= PATTERN_COUNT)
        {
            currentIndex = 0;
        }
    }
    void setup()
    {
        effects = new Effects();
        effects->Setup();
    }

    void showFrame()
    {
        unsigned long now = millis();
        if (now > isRenderingDue)
        {
            isRenderingDue = now + renderingDelay;
            renderFrame();
        }
        effects->ShowFrame();
    }

    ~Playlist()
    {
        free(effects);
    }
};

#endif