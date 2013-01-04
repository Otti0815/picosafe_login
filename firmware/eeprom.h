#ifndef __EPROM_H_
#define __EPROM_H_

#include <stdint.h>
#include <avr/io.h>

uint8_t eeprom_write(uint16_t addr, char * buf, uint16_t length);
uint8_t eeprom_read(uint16_t addr, char * buf, uint16_t length);

#endif
