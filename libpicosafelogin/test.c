#include <stdio.h>
#include <string.h>
#include "libpicosafelogin.h"

#define PASSWORD "biijoov1vedex3Xigohng7quoovohTieNgoh9ahnil"
#define PASSWORD2 "biijoov1vedex3Xigohng7quoovohTieNgoh9ahnil"

int main(int argc, char *argv[])
{
  uint32_t timestamp = 1337;
  int num = 0;

  if(argc < 2)
  {
    int i = picosafelogin_set_seed(PASSWORD);
    printf("set_seed %d\n", i);
  }
  else
  {
    picosafelogin_set_timestamp(timestamp, PASSWORD2);
    int j = picosafelogin_verify_token(argv[1], PASSWORD, &num, &timestamp);
    printf("picosafelogin_verify_token %d, num %d, timestamp %d\n", j, num, timestamp);
  }
  
  return 0;
}
