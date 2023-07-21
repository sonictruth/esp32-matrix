#include "config.h"
#include <arduinoFFT.h>

#define AUDIO_IN_PIN 34     // Signal in on this pin

#define SAMPLES 64          // Must be a power of 2
#define SAMPLING_FREQ 40000 // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE 400       // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.

#define NUM_BANDS 64
#define NOISE 500
#define TOP 32

unsigned int sampling_period_us;
byte peak[NUM_BANDS];
int oldBarHeights[NUM_BANDS];
int bandValues[NUM_BANDS];
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void setupFFT()
{
    pinMode(AUDIO_IN_PIN, INPUT);
    sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
}

void showFFTFrame()
{
    // Reset bandValues[]
    for (int i = 0; i < NUM_BANDS; i++)
    {
        bandValues[i] = 0;
    }
    // Sample the audio pin
    for (int i = 0; i < SAMPLES; i++)
    {
        newTime = micros();
        long sample = analogRead(AUDIO_IN_PIN);
        long sampleMin = 1200; // 2000 - 3000
        long sampleMax = 3000;

        sample = max(sample, sampleMin);
        sample = min(sample, sampleMax);
        uint16_t value = map(sample, sampleMin, sampleMax, 0, 4096);
        vReal[i] = value;
        vImag[i] = 0;
        while ((micros() - newTime) < sampling_period_us)
        { /* chill */
        }
    }
    // Compute FFT
    FFT.DCRemoval();
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
    for (int i = 0; i < SAMPLES; i++)
    { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
        bandValues[i] += (int)vReal[i];
    }
    // dma_display->clearScreen();
    for (byte band = 0; band < NUM_BANDS; band++)
    {

        // Scale the bars for the display
        // int barHeight = bandValues[band] / AMPLITUDE;
        int barHeight = map(bandValues[band], 0, 11000, 0, 20);
        // if (barHeight > TOP)
        //   barHeight = TOP;

        // Small amount of averaging between frames
        // barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

        // Move peak up
        if (barHeight > peak[band])
        {
            peak[band] = min(TOP, barHeight);
        }
        Serial.println(barHeight);
        // dma_display->drawFastVLine(band, 0, barHeight, xw);
        // dma_display->drawPixel(band, peak[band], myWHITE);
        oldBarHeights[band] = barHeight;
    }
}
