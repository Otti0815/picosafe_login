#include <avr/io.h>
#include <stdint.h>

#include "utils.h"
#include "date.h"
#include "i2c.h"
#include "rtc.h"

// RTC
void rtc_init(void)
{
  //uint8_t control_status;
  i2c_init();

  /*
  control_status = rtc_read(RTC_PAGE_CONTROL | RTC_CONTROL_INT_FLAGS);
  control_status &= ~(1 << 5);
  rtc_write(RTC_PAGE_CONTROL | RTC_CONTROL_INT_FLAGS, control_status);
  */
}

void rtc_read_page(uint8_t start_register, uint8_t *buffer, uint8_t size)
{
  uint8_t i;

  i2c_start(); // 1

  i2c_write(RTC_ADDRESS | I2C_WRITE); // 2+3
  i2c_write(start_register); // 4+5

  i2c_stop(); // 6

  i2c_start(); // 7
  i2c_write(RTC_ADDRESS | I2C_READ); // 8+9

  for(i = 0; i < size; i++)
    buffer[i] = i2c_read((i == (size-1)) ? I2C_NACK : I2C_ACK);

  i2c_stop(); // 6
}

uint8_t rtc_read(uint8_t r)
{
  uint8_t buf;
  rtc_read_page(r, &buf, 1);
  return buf;
}

void rtc_write(uint8_t r, uint8_t data)
{
  uint8_t buf = data;
  rtc_write_page(r, &buf, 1);
}

void rtc_write_page(uint8_t start_register, uint8_t *buffer, uint8_t size)
{
  uint8_t i;

  i2c_start(); // 1

  i2c_write(RTC_ADDRESS | I2C_WRITE); // 2+3
  i2c_write(start_register); // 4+5

  for(i = 0; i < size; i++)
    i2c_write(buffer[i]); // 8+9

  i2c_stop(); // 6
}

uint32_t rtc_get_time(void)
{
  uint8_t i;//, pm = 0;
  // 0        1        2      3     4         5       6
  // seconds, minutes, hours, days, weekdays, months, years
  uint8_t date[7];

  if(rtc_get_pon() == 1)
    return 0;

  rtc_read_page(RTC_PAGE_CLOCK, date, 7);

  //if(date[2] & (1<< 6))
  //  PORTB |= (1 << PB4);
  //  pm = 1;
  date[0] &= 0x7f;
  date[1] &= 0x7f;
  date[2] &= 0x3f;
  date[3] &= 0x3f;
  date[4] &= 0x07;
  date[5] &= 0x3f;
  date[6] &= 0x7f;

  for(i = 0; i < 7; i++)
    date[i] = bcd2int(date[i]);

  //if(pm)
  //  date[2] += 12;

  return date_convert2timestamp((int16_t)date[6]+2000, date[5], date[3], date[2], date[1], date[0]);
}

void rtc_set_time(uint32_t timestamp)
{
  int16_t year, doy;
  int8_t hour, min, sec, day, mon, isdst, weekday;
  // 0        1        2      3     4         5       6
  // seconds, minutes, hours, days, weekdays, months, years
  uint8_t date[7];
  uint8_t control_status;

  // nach utc
  date_timestamp2human(timestamp, 0, &year, &doy, &hour, &min, &sec, &isdst);
  date_doy2daymon(doy, year, &day, &mon);
  weekday = date_wday(doy, year) + 1;

  date[0] = int2bcd(sec);
  date[1] = int2bcd(min);
  date[2] = int2bcd(hour);
  date[3] = int2bcd(day);
  date[4] = int2bcd(weekday);
  date[5] = int2bcd(mon);
  date[6] = int2bcd(year-2000);

  rtc_write_page(RTC_PAGE_CLOCK, date, 7);

  control_status = rtc_read(RTC_PAGE_CONTROL | RTC_CONTROL_INT_FLAGS);
  control_status &= ~(1 << 5);
  rtc_write(RTC_PAGE_CONTROL | RTC_CONTROL_INT_FLAGS, control_status);
}

uint8_t rtc_get_pon(void)
{
  uint8_t control_status;

  control_status = rtc_read(RTC_PAGE_CONTROL | RTC_CONTROL_INT_FLAGS);

  if(control_status & (1 << 5))
    return 1;
  else
    return 0;
}

int8_t rtc_get_temperature(void)
{
  return rtc_read(RTC_PAGE_TEMPERATURE)-60;
}
