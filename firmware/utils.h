#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

void sleep_ms(uint16_t time);
void sleep_us(uint16_t time);

uint8_t bcd2int(uint8_t bcd);
uint8_t int2bcd(uint8_t value);

uint32_t array2int(uint8_t *buf);
void int2array(uint8_t *buf, uint32_t *i);

#endif // _UTILS_H_
