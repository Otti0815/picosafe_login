#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "date.h"

#define Y2010 1262304000 // timestamp 01.01.2010 00:00:00 UTC
#define DAY   86400      // seconds per day

/* gibt zurueck, ob jahr schaltjahr ist (return: 1) oder nicht (return: 0)
 * ein Jahr ist ein Schaltjahr, wenn:
 * Regel: 
 * Jahr ist ganzzahlig durch 4 teilbar                     => Schaltjahr 
 * Ausname: 
 * Jahr ist auch gleichzeitig ganzzahlig durch 100 teilbar => kein Schaltjahr
 * Ausnahme der Ausnahme: 
 * Jahr ist ganzzahlig durch 400 teilbar                   => Schaltjahr
 */
int8_t date_isleapyear(int16_t year)
{
  if(year % 400 == 0)
    return 1;
  else if(year % 100 == 0)
    return 0;
  else if(year % 4 == 0)
    return 1;
  else
    return 0;
}

/* gibt wochentag fuer bestimmten tag eines jahres zurueck
 * 0: sonntag, 1: montag, 2: dienstag...
 */
int8_t date_wday(int16_t doy, int16_t year)
{
  int8_t y = year % 100;
  int8_t doomsday_year = (y/12 + y % 12 + (y % 12)/4)% 7;

  if(year >= 2000)
    doomsday_year += 2;
  else // 19XX
    doomsday_year += 3;


  if(!date_isleapyear(year))
    doomsday_year -= 3;
  else
    doomsday_year -= 4;

  return (doomsday_year + doy) % 7;
}

uint8_t date_days_in_month(uint8_t month, uint8_t year)
{
                // Jan, Feb,                 Mar, Apr, Mai, Jun, Jul, Aug, Sep, Okt, Nov, Dez
  const int8_t days[] = { 31, 28+date_isleapyear(year), 31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };

  return days[month];
}

/* wandelt tag eines jahres in monatstag und monat um */
void date_doy2daymon(int16_t doy, int16_t year, int8_t *day, int8_t *mon)
{
  uint8_t i = 0;

  // monat bestimmen
  while((doy-date_days_in_month(i, year)) > 0)
    doy -= date_days_in_month(i++, year);

  *mon = i+1; // monat
  *day = doy; // tag des monats
}

void date_timestamp2human(uint32_t timestamp, int8_t localtime, int16_t *year, int16_t *doy, int8_t *hour, int8_t *min, int8_t *sec, int8_t *isdst)
{
  int16_t y = 1970;
  int16_t doy_dst_start, doy_dst_end, d;
  int8_t dst, h, m, s;

  // performance improvement
  if(timestamp > (uint32_t)Y2010)
  {
    timestamp -= Y2010;
    y = 2010;
  }

  while(1)
  {
    uint32_t seconds_in_year = (365+date_isleapyear(y))*DAY;

    if((int32_t)(timestamp - seconds_in_year) >= 0)
    {
      // ein weiteres jahr vergangen
      timestamp -= seconds_in_year;
      y++;
    }
    else
      break;
  }

  // day of year
  d = timestamp/DAY+1;

  // sekunden seit tagesbeginn (00:00)
  timestamp %= DAY;

  // hour
  h = timestamp / 3600;
  // min
  m = (timestamp % 3600)/60;
  // sec
  s = timestamp % 60;

  // DST
  if(localtime)
  {
    //              Jan, Feb,               Mar
    doy_dst_start = 31 + 28+date_isleapyear(y) + 31; // 31.03.
    while(date_wday(doy_dst_start, y) != 0)        // letzten sonntag suchen
      doy_dst_start--;

    //              Jan, Feb                Mar, Apr, Mai, Jun, Jul, Aug, Sep, Okt
    doy_dst_end   = 31 + 28+date_isleapyear(y) + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31; // 31.10.
    while(date_wday(doy_dst_end, y) != 0)          // letzten sonntag suchen
      doy_dst_end--;

    if(d > doy_dst_start && d < doy_dst_end)
      dst = 1;
    else if(d == doy_dst_start && h >= 1)
      dst = 1;
    else if(d == doy_dst_end && h <= 1)
      dst = 1;
    else
      dst = 0;

  /********* bis hierhin sind alle zeitangaben UTC **********/

  // UTC in lokalzeit umrechnen
  if(dst)
    h += 2; // MESZ: UTC+2
  else
    h += 1; // MEZ:  UTC+1
  }
  else
    dst = 0;

  // wenn sich ein tagumbruch ergeben hat...
  if(h>23)
  {
    h %= 24;
    d++;

    // jahrumbruch...
    if(d > (365+date_isleapyear(y)))
    {
      d = 1;
      y++;
    }
  }

  if(year != NULL)
    *year = y;
  if(doy != NULL)
    *doy = d;
  if(hour != NULL)
    *hour = h;
  if(min != NULL)
    *min = m;
  if(sec != NULL)
    *sec = s;
  if(isdst != NULL)
    *isdst = dst;
}

uint32_t date_convert2timestamp(uint16_t year, uint8_t month, uint8_t dom, uint8_t hour, uint8_t min, uint8_t sec)
{
  uint32_t timestamp = Y2010;
  uint16_t i;

  if(year >= 2010)
  {
    for(i = 2010; i < year; i++)
    {
      if(date_isleapyear(i))
        timestamp += 366*DAY;
      else
        timestamp += 365*DAY;
    }
  }
  else
    return -1; // fehler


  if(month > 1)
    for(i = 0; i < month-1; i++)
      timestamp += date_days_in_month(i, year) * DAY;

  if(dom > 1)
    timestamp += (dom-1)*DAY;


  timestamp += (uint32_t)hour*3600;
  timestamp += (uint16_t)min*60;
  timestamp += sec;

  return timestamp;
}
