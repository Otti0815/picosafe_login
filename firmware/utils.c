#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

void sleep_ms(uint16_t time)
{
  while(time--)
    _delay_ms(1);
}

void sleep_us(uint16_t time)
{
  while(time--)
    _delay_us(1);
}

uint8_t bcd2int(uint8_t bcd)
{
  uint8_t value;

  value = bcd & 0x0F;
  value += (bcd >> 4)*10;

  return value;
}

uint8_t int2bcd(uint8_t value)
{
  uint8_t bcd = 0;

  if(value >= 80)
  {
    value -= 80;
    bcd |= (1 << 7);
  }
  if(value >= 40)
  {
    value -= 40;
    bcd |= (1 << 6);
  }
  if(value >= 20)
  {
    value -= 20;
    bcd |= (1 << 5);
  }
  if(value >= 10)
  {
    value -= 10;
    bcd |= (1 << 4);
  }

  bcd |= (value & 0x0F);

  return bcd;
}

uint32_t array2int(uint8_t *buf)
{
  return ((uint32_t)buf[0] << 24) |
         ((uint32_t)buf[1] << 16) |
         ((uint32_t)buf[2] << 8 ) |
                    buf[3];
}

void int2array(uint8_t *buf, uint32_t *i)
{
  buf[0] = *i >> 24;
  buf[1] = *i >> 16;
  buf[2] = *i >>  8;
  buf[3] = *i;
}
