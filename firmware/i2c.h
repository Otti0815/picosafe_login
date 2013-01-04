#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>

#define I2C_READ        0x01
#define I2C_WRITE       0x00

#define I2C_ACK  1
#define I2C_NACK 0

#define I2C_DDR   DDRD
#define I2C_PORT  PORTD
#define I2C_PIN   PIND
#define I2C_SDA   PD1
#define I2C_SCL   PD0
#define I2C_DELAY 10

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t byte);
uint8_t i2c_read(uint8_t ack);

#endif
