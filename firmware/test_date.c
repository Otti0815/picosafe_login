#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "date.h"

/* hardcore testen fuer jede sekunde zwischen 2010-2030:
   timestamp1 => humand readable => timestamp2: timestamp1 == timestamp2?
   gcc -Wall -O4 date.c test_date.c -o test_date
 */

int main(void)
{
  int16_t year, doy;
  int8_t hour, min, sec, day, mon, isdst, weekday;
  uint32_t start = 1262304000, end = 1893456000, in, out;

  // zeitraum 2010-2030
  for(in = start; in < end; in++)
  {
    out = 0;
    if(in % 1000000 == 0)
      printf("%d/%d\n", in, end);

    date_timestamp2human(in, 0, &year, &doy, &hour, &min, &sec, &isdst);
    date_doy2daymon(doy, year, &day, &mon);
    weekday = date_wday(doy, year) + 1;
    out = date_convert2timestamp(year, mon, day, hour, min, sec);

    if(in != out)
    {
      fprintf(stderr, "fail %d -> %d\n", in, out);
      exit(1);
    }
  }

  printf("ok\n");

  return 0;
}
