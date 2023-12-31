#include <stdio.h>
#include <arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *dma_display;

uint16_t getRandomColor()
{
  return dma_display->color565(
      random(128, 256),
      random(128, 256),
      random(1, 128));
}

void getTimeEnglish(char *result, int h, int m)
{
  if (h > 12)
  {
    h = h - 12;
  }
  char nums[][64] = {
      "zero",
      "one",
      "two",
      "three",
      "four",
      "five",
      "six",
      "seven",
      "eight",
      "nine",
      "ten",
      "eleven",
      "twelve",
      "thirteen",
      "fourteen",
      "fifteen",
      "sixteen",
      "seventeen",
      "eighteen",
      "nineteen",
      "twenty",
      "twenty one",
      "twenty two",
      "twenty three",
      "twenty four",
      "twenty five",
      "twenty six",
      "twenty seven",
      "twenty eight",
      "twenty nine",
  };

  if (m == 0)
    sprintf(result, "%s o' clock", nums[h]);

  else if (m == 1)
    sprintf(result, "one minute past %s", nums[h]);

  else if (m == 59)
    sprintf(result, "one minute to %s", nums[(h % 12) + 1]);

  else if (m == 15)
    sprintf(result, "quarter past %s", nums[h]);

  else if (m == 30)
    sprintf(result, "half past %s", nums[h]);

  else if (m == 45)
    sprintf(result, "quarter to %s", nums[(h % 12) + 1]);

  else if (m <= 30)
    sprintf(result, "%s minutes past %s", nums[m], nums[h]);

  else if (m > 30)
    sprintf(result, "%s minutes to %s", nums[60 - m],
            nums[(h % 12) + 1]);
}
