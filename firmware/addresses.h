#ifndef __ADDRESSES_H_
#define __ADDRESSES_H_

#define ADDR_INIT        1
#define ADDR_LOCK        2    // [1-1]:    1 byte
#define ADDR_COUNTER     3    // [2-3]:  2 bytes
#define ADDR_SEED        5    // [4-51]: 48 bytes
#define ADDR_ID          53   // [52-55]:    4 bytes
#define ADDR_COMPANY     57   // [56-106]:   50 chars + NUL
#define ADDR_OWNER       108  // [107-157]: 50 chars + NUL
#define ADDR_DESCRIPTION 159  // [158-208]: 50 chars + NUL

#define LEN_COMPANY       51
#define LEN_ID            4
#define LEN_OWNER         51
#define LEN_TIMESTAMP     4
#define LEN_SEED          42
#define LEN_DESCRIPTION  51
#define LEN_COUNTER     2

#endif // __ADDRESSES_H_
