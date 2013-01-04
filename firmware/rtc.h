#ifndef __RTC_H
#define __RTC_H

#include <stdint.h>

#define RTC_REGISTER_TEMPERATURE 0x20
#define RTC_REGISTER_EEPROM_CONTROL 0x30
#define RTC_PAGE_CLOCK           0x08
#define RTC_PAGE_CONTROL         0x00
#define RTC_PAGE_TEMPERATURE     0x20
#define RTC_CONTROL_INT_FLAGS    0x03

#define RTC_ADDRESS              0xAC

int8_t rtc_get_temperature(void);
uint8_t rtc_get_seconds(void);
void rtc_read_page(uint8_t start_register, uint8_t *buffer, uint8_t size);
void rtc_write_page(uint8_t start_register, uint8_t *buffer, uint8_t size);
void rtc_write(uint8_t r, uint8_t data);
uint8_t rtc_read(uint8_t r);
uint32_t rtc_get_time(void);
void rtc_set_time(uint32_t timestamp);
uint8_t rtc_get_pon(void);
void rtc_init(void);

#endif
