/*
 * (c) 2009, Benedikt Sauter, Michael Hartmann
 * original taken from usbprog source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <usb.h>
#include <unistd.h>

#include "libpicosafelogin.h"
#include "base64.h"
#include "rijndael.h"

#include "../firmware/common.h"
#include "../firmware/addresses.h"

#define USB_WRITE 0x40
#define USB_READ  0xC0

#define BLOCKSIZE 16
#define KEYSIZE   32
#define DATASIZE  14
#define MAXPAD    33
#define KEYBITS   256

#define TIMEOUT 100

static unsigned char *encrypt(unsigned char *ciphertext, unsigned char *plaintext, const unsigned char *key)
{
  unsigned long rk[RKLENGTH(KEYBITS)];

  int nrounds = rijndaelSetupEncrypt(rk, key, KEYBITS);

  rijndaelEncrypt(rk, nrounds, ciphertext, plaintext);

  return ciphertext;
}

static unsigned char *decrypt(unsigned char *crypted, const unsigned char *key)
{
  unsigned long rk[RKLENGTH(KEYBITS)];
  unsigned char plain[BLOCKSIZE];

  int nrounds = rijndaelSetupDecrypt(rk, key, KEYBITS);

  rijndaelDecrypt(rk, nrounds, crypted, plain);

  int i;
  for(i = 0; i < 16; i++)
    crypted[i] = plain[i];

  crypted[i] = '\0';

  return crypted;
}


static struct usb_dev_handle *picosafelogin_open(void)
{
  usb_init();
  usb_find_busses();
  usb_find_devices();

  struct usb_bus *bus = usb_get_busses ();
  for( ; bus; bus = bus->next)
  {
    struct usb_device *dev;
    for (dev = bus->devices; dev; dev = dev->next)
    {
      if (dev->descriptor.idVendor == OPENKUBUS_VID && dev->descriptor.idProduct == OPENKUBUS_PID)
        return usb_open(dev);
    }
  }
  return NULL;
}


static void picosafelogin_close(struct usb_dev_handle *usb_handle)
{
  usb_close(usb_handle);
}


static int picosafelogin_control_msg(int bmRequestType, int bRequest, char *buffer, int len)
{
  char ret = 0;
  struct usb_dev_handle *usb_handle = picosafelogin_open();

  if(usb_handle == NULL)
    return -1;

  ret = usb_control_msg(usb_handle, bmRequestType, bRequest, 0, 0, buffer, len, TIMEOUT);

  if(bmRequestType == USB_WRITE)
  {
    ret = -1;

    usleep(200000);
    usb_control_msg(usb_handle, USB_READ, OPENKUBUS_GET_SUCCESS, 0, 0, &ret, sizeof(ret), TIMEOUT);
  }

  picosafelogin_close(usb_handle);

  return ret;
}


int picosafelogin_set_seed(char *seed)
{
  if(strlen(seed) != LEN_SEED)
    return 0;

  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_SET_SEED, seed, strlen(seed));
}


int picosafelogin_set_company(char *company)
{
  if(strlen(company) > LEN_COMPANY)
    return 0;

  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_SET_COMPANY, company, strlen(company)+1);
}

int picosafelogin_set_owner(char *owner)
{
  if(strlen(owner) > LEN_OWNER)
    return 0;

  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_SET_OWNER, owner, strlen(owner)+1);
}


int picosafelogin_get_company(char *company)
{
  return picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_COMPANY, company, LEN_COMPANY);
}


int picosafelogin_get_owner(char *owner)
{
  return picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_OWNER, owner, LEN_OWNER);
}


int picosafelogin_set_description(char *description)
{
  if(strlen(description) > LEN_DESCRIPTION)
    return 0;

  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_SET_DESCRIPTION, description, strlen(description)+1);
}


int picosafelogin_get_description(char *description)
{
  return picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_DESCRIPTION, description, LEN_DESCRIPTION);
}


int picosafelogin_set_id(uint32_t id)
{
  char data[4];

  data[0] = id >> 24;
  data[1] = id >> 16;
  data[2] = id >> 8;
  data[3] = id;

  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_SET_ID, data, sizeof(data));
}


int picosafelogin_get_time(uint32_t *time)
{
  int ret;
  char input[4];
  memset(input, 0, sizeof(input));

  ret = picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_TIME, input, 4);

  *time = ((uint8_t)input[0] << 24) |
          ((uint8_t)input[1] << 16) |
          ((uint8_t)input[2] << 8 ) |
          (uint8_t)input[3];

  return ret;
}


int picosafelogin_get_internal_serial(char serial[11])
{
  memset(serial, 0, sizeof(serial));

  return picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_SERIAL, serial, 11);
}


int picosafelogin_get_id(uint32_t *id)
{
  int ret;
  char input[4];
  memset(input, 0, sizeof(input));

  ret = picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_ID, input, 4);
  *id = (uint8_t)input[0] << 24 |
        (uint8_t)input[1] << 16 |
        (uint8_t)input[2] << 8  |
        (uint8_t)input[3];

  return ret;
}


int picosafelogin_get_nonce(char *nonce, int length)
{
  return picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_NONCE, nonce, length);
}


int picosafelogin_reset(char *master_password)
{
  char data[16];
  char cipher[16];
  char key[KEYSIZE+1];

  strncpy(key, master_password, KEYSIZE);
  key[KEYSIZE] = 0;

  picosafelogin_get_nonce(data, sizeof(data));

  encrypt((unsigned char *)data, (unsigned char *)cipher, (unsigned char *)key);

  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_RESET, cipher, sizeof(cipher));
}


int picosafelogin_set_timestamp(uint32_t timestamp, const char *pw)
{
  char key[KEYSIZE+1];
  char data[16];
  char cipher[16];

  strncpy(key, pw, KEYSIZE);
  key[KEYSIZE] = 0;

  picosafelogin_get_nonce(data, sizeof(data));
  
  data[12] = timestamp >> 24;
  data[13] = timestamp >> 16;
  data[14] = timestamp >> 8;
  data[15] = timestamp;

  encrypt((unsigned char *)data, (unsigned char *)cipher, (unsigned char *)key);

  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_SET_TIMESTAMP, cipher, sizeof(cipher));
}


int picosafelogin_get_temperature()
{
  char temperature = 0;
  picosafelogin_control_msg(USB_READ, OPENKUBUS_GET_TEMPERATURE, &temperature, 1);

  return temperature;
}


int picosafelogin_set_lock()
{
  return picosafelogin_control_msg(USB_WRITE, OPENKUBUS_SET_LOCK, NULL, 0);
}



/*
// converts real base64 to pad
static void base642pad(char *str)
{
  int i;

  for(i = 0; i < strlen(str); i++)
  {
    switch(str[i])
    {
      case '/': str[i] = '!'; break;
      case '=': str[i] = '.'; break;
      case '+': str[i] = '-'; break;
    }
  }
}
*/


// converts string to real base64
static void pad2base64(char *str)
{
  char a = str[0];
  int i;

  for(i = 0; i < strlen(str)-1; i++)
  {
    str[i] = str[i+1];

    switch(str[i])
    {
      case '!': str[i] = '/'; break;
      case '.': str[i] = '='; break;
      case '-': str[i] = '+'; break;
    }

    if(a == 'y' || a == 'Y')
    {
      switch(str[i])
      {
        case 'y': str[i] = 'z'; break;
        case 'Y': str[i] = 'Z'; break;
        case 'z': str[i] = 'y'; break;
        case 'Z': str[i] = 'Y'; break;
      }
    }
  }
  str[i] = '\0';
}


/*
void picosafelogin_gen_pad(const char *pw, int offset, int num, char *pad)
{
  char data[17];
  char aes[33];
  char crypted[33];
  uint8_t i;

  num += offset;

  data[0] = (uint8_t) num;
  data[1] = (uint8_t) (num >> 8);

  for(i = 0; i < 14; i++)
    data[2+i] = pw[32+i];
  data[16] = 0;

  for(i = 0; i < 32; i++)
    aes[i] = pw[i];
  aes[32] = 0;

  encrypt((unsigned char *)data, (unsigned char *)crypted, (unsigned char *)aes);
  crypted[33] = 0;

  pad[0] = 'z';
  i = raw_to_base64(crypted, 16, &pad[1], 60);
  pad[i+1] = 0;
  base642pad(pad);
}
*/

int picosafelogin_verify_token(const char *pad, const char *pw, int *num, uint32_t *timestamp)
{
  char *plain = NULL;
  int len;
  char key[KEYSIZE+1];
  char data[DATASIZE+1];
  char str[MAXPAD+1];

  if((len = strlen(pad)) < BLOCKSIZE)
    return -1;

  if(strlen(pw) != 32+10)
    return -2;

  if(strlen(pad) > MAXPAD)
    return -3;

  strncpy(key, pw, KEYSIZE);
  key[KEYSIZE] = '\0';

  strncpy(data, &pw[KEYSIZE], DATASIZE);
  data[DATASIZE] = 0;

  memcpy(str, pad, strlen(pad)+1);

  pad2base64(str);

  if(base64_to_raw_inplace(str, len) != BLOCKSIZE)
    return -5;

  plain = (char *)decrypt((unsigned char *)str, (unsigned char *)key);

  *num  =  (uint8_t)  plain[0];
  *num |= ((uint16_t) plain[1] << 8);

  if(strncmp(plain+2, data, 10) != 0)
    return -12;

  *timestamp = (uint8_t)plain[12] << 24
             | (uint8_t)plain[13] << 16
             | (uint8_t)plain[14] << 8
             | (uint8_t)plain[15];

  return 1;
}
