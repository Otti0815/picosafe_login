/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for Keyboard.c.
 */

#ifndef _OPENKUBUS_H_
#define _OPENKUBUS_H_

#define TWBR _SFR_MEM8(0xB8)

#define TWBR0 0

#define TWBR1 1

#define TWBR2 2

#define TWBR3 3

#define TWBR4 4

#define TWBR5 5

#define TWBR6 6

#define TWBR7 7



#define TWSR _SFR_MEM8(0xB9)

#define TWPS0 0

#define TWPS1 1

#define TWS3 3

#define TWS4 4

#define TWS5 5

#define TWS6 6

#define TWS7 7



#define TWAR _SFR_MEM8(0xBA)

#define TWGCE 0

#define TWA0 1

#define TWA1 2

#define TWA2 3

#define TWA3 4

#define TWA4 5

#define TWA5 6

#define TWA6 7



#define TWDR _SFR_MEM8(0xBB)

#define TWD0 0

#define TWD1 1

#define TWD2 2

#define TWD3 3

#define TWD4 4

#define TWD5 5

#define TWD6 6

#define TWD7 7



#define TWCR _SFR_MEM8(0xBC)

#define TWIE 0

#define TWEN 2

#define TWWC 3

#define TWSTO 4

#define TWSTA 5

#define TWEA 6

#define TWINT 7



#define TWAMR _SFR_MEM8(0xBD)

#define TWAM0 1

#define TWAM1 2

#define TWAM2 3

#define TWAM3 4

#define TWAM4 5

#define TWAM5 6

#define TWAM6 7

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		#include <string.h>
    #include <avr/pgmspace.h>
    #include <stdlib.h>
    #include <util/delay.h>
		
    #include "Descriptors.h"

		#include <LUFA/Version.h>
		#include <LUFA/Drivers/USB/USB.h>
    #include <LUFA/Drivers/USB/LowLevel/Endpoint.h>
    #include <LUFA/Drivers/Peripheral/SerialStream.h>

    #ifndef MIN
        #define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
    #endif

	/* Function Prototypes: */
		void SetupHardware(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);
		void EVENT_USB_Device_StartOfFrame(void);

		bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
		                                         uint8_t* const ReportID,
		                                         const uint8_t ReportType,
		                                         void* ReportData,
		                                         uint16_t* const ReportSize);
		void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
		                                          const uint8_t ReportID,
		                                          const uint8_t ReportType,
		                                          const void* ReportData,
		                                          const uint16_t ReportSize);

    void timer_init(void);
    uint32_t get_time(void);
    void token(void);
    unsigned short get_seed(void);
    void update_nonce(void);
    void char2usb(uint8_t *hex, uint8_t *modifier);
    void clear_eeprom(void);
#endif
