#include <avr/io.h>
#include <stdint.h>

#include "i2c.h"
#include "utils.h"

static void inline i2c_sda_low(void)
{
  I2C_PORT &= ~(1 << I2C_SDA);
  I2C_DDR  |= (1 << I2C_SDA);
}

static void inline i2c_sda_high(void)
{
  I2C_DDR  &= ~(1 << I2C_SDA);
  I2C_PORT |=  (1 << I2C_SDA);
}

static void inline i2c_scl_high(void)
{
  I2C_PORT |= (1 << I2C_SCL);
}

static void inline i2c_scl_low(void)
{
  I2C_PORT &= ~(1 << I2C_SCL);
}

void static inline i2c_scl_toggle(void)
{
  i2c_scl_high();
  sleep_us(I2C_DELAY);
  i2c_scl_low();
  sleep_us(I2C_DELAY);
}


void i2c_init(void)
{
  I2C_DDR |= ((1 << I2C_SCL) | (1 << I2C_SDA));
  i2c_sda_high(); // sollte schon hi sein
  i2c_scl_high(); // sollte schon hi sein
}


void i2c_start(void)
{
  i2c_sda_high(); // sollte schon hi sein
  i2c_scl_high(); // sollte schon hi sein

  sleep_us(I2C_DELAY);

  i2c_sda_low();

  sleep_us(I2C_DELAY);
  i2c_scl_low(); // sollte schon hi sein
  sleep_us(I2C_DELAY);
}

void i2c_stop(void)
{
  i2c_sda_low();
  i2c_scl_high();
  sleep_us(I2C_DELAY);
  i2c_sda_high();
  sleep_us(I2C_DELAY);
}

void i2c_write(uint8_t byte)
{
  uint8_t i;

  for(i = 0; i < 8; i++)
  {
    if (byte & 0x80)
      i2c_sda_high();
    else
      i2c_sda_low();

    byte = byte<<1;
    sleep_us(I2C_DELAY);
    i2c_scl_toggle();
  }

  // Auf ACK vom Slave Prüfen
  i2c_sda_high();
  sleep_us(I2C_DELAY);
  i2c_scl_toggle();
}


uint8_t i2c_read(uint8_t ack)
{
  uint8_t i, c = 0;

  i2c_sda_high();

  for(i = 0; i < 8; i++)
  {
    c = c << 1;

    if(I2C_PIN & (1 << I2C_SDA))
      c |= 1;

    i2c_scl_toggle();
  }

  if(ack)
    i2c_sda_low();
  else
    i2c_sda_high();

  i2c_scl_toggle();

  return c;
}
