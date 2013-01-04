#ifndef _DATE_H_
#define _DATE_H_

#include <stdint.h>

int8_t date_isleapyear(int16_t year);
int8_t date_wday(int16_t doy, int16_t year);
uint8_t date_days_in_month(uint8_t month, uint8_t year);
void date_doy2daymon(int16_t doy, int16_t year, int8_t *day, int8_t *mon);
void date_timestamp2human(uint32_t timestamp, int8_t localtime, int16_t *year, int16_t *doy, int8_t *hour, int8_t *min, int8_t *sec, int8_t *isdst);
uint32_t date_convert2timestamp(uint16_t year, uint8_t month, uint8_t dom, uint8_t hour, uint8_t min, uint8_t sec);

#endif // _DATE_H_
