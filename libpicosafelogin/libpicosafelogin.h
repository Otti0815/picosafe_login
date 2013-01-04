#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>

#define OPENKUBUS_VID 0x1781
#define OPENKUBUS_PID 0x0c66

int picosafelogin_set_owner(char *owner);
int picosafelogin_get_owner(char *owner);
int picosafelogin_get_company(char *company);
int picosafelogin_set_company(char *company);
int picosafelogin_set_seed(char *password);
int picosafelogin_set_description(char *description);
int picosafelogin_get_description(char *description);
int picosafelogin_set_id(uint32_t id);
int picosafelogin_get_id(uint32_t *id);
int picosafelogin_get_time(uint32_t *time);
int picosafelogin_get_temperature(void);
int picosafelogin_set_lock(void);
int picosafelogin_verify_token(const char *pad, const char *pw, int *num, uint32_t *timestamp);
int picosafelogin_get_nonce(char *nonce, int length);
int picosafelogin_set_timestamp(uint32_t timestamp, const char *pw);
int picosafelogin_reset(char *master_password);
int picosafelogin_get_internal_serial(char serial[12]);


#endif
