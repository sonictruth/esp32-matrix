#include <stdio.h>
#include <arduino.h>

String timeToEnglish(int h, int m)
{
  if (h > 12) {
    h = h - 12;
  }
  char nums[][64] = { "zero", "one", "two", "three", "four",
                      "five", "six", "seven", "eight", "nine",
                      "ten", "eleven", "twelve", "thirteen",
                      "fourteen", "fifteen", "sixteen", "seventeen",
                      "eighteen", "nineteen", "twenty", "twenty one",
                      "twenty two", "twenty three", "twenty four",
                      "twenty five", "twenty six", "twenty seven",
                      "twenty eight", "twenty nine",
                    };
  char result[100];
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

  return String(result);
}
