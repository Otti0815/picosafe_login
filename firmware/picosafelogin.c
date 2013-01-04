#include "picosafelogin.h"

/* headers for encrypting/decrypting AES256 */
#include "aes/aes.h"
#include "aes/aes256_dec.h"
#include "aes/aes256_enc.h"
#include "aes/aes_dec.h"
#include "aes/aes_enc.h"
#include "aes/aes_invsbox.h"
#include "aes/aes_keyschedule.h"
#include "aes/aes_sbox.h"
#include "aes/gf256mul.h"

#include "addresses.h"
#include "base64.h"
#include "common.h"
#include "eeprom.h"
#include "fifo.h"
#include "utils.h"

#define RTC 1

#ifdef RTC
#include "rtc.h"
#endif

#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED      PB4

#define BUTTON_DDR DDRE
#define BUTTON_PIN PINE
#define BUTTON     PE2 // KETEK PE2 old PE6

/* minimum time after button press before responding to another press (in 20ms;
 * 50 == 1s) */
#define DELTA_T 50

const prog_char MASTER_PASSWORD[] = "coa7Heeh8ChaiR9XaikeeRei4fuo2kah";

fifo_t fifo;             // fifo storing keys to send to host
uint8_t fifo_buffer[50]; // buffer for fifo
uint32_t time = 0;       // in 20ms; e.g. 100 => 2s
char nonce[5];           // nonce

/* if we have a RTC, use rtc-time; else set timestamp=0 */
#ifndef RTC
uint32_t timestamp = 0;
#endif

void set_timestamp(uint32_t time)
{
  #ifdef RTC
    rtc_set_time(time);
  #else
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
      timestamp = time;
    }
  #endif
}

uint32_t get_timestamp(void)
{
  uint32_t time;

  #ifdef RTC
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
      time = rtc_get_time();
    }
  #else
    time = timestamp;
  #endif

  return time;
}

/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
   {
    .Config =
      {
        .InterfaceNumber              = 0,

        .ReportINEndpointNumber       = KEYBOARD_EPNUM,
        .ReportINEndpointSize         = KEYBOARD_EPSIZE,
        .ReportINEndpointDoubleBank   = false,

        .PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
        .PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
      },
    };

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
  uint32_t last = 0;

  /* init fifo */
  fifo_init(&fifo, fifo_buffer, sizeof(fifo_buffer));

  /* setup hardware (button, LED, rtc...) */
  SetupHardware();

  /* enable interrupts */
  sei();


  /* main loop */
  while(1)
  {
    /* if button pressed */
    if (!(BUTTON_PIN & (1 << BUTTON)))
    {
      /* ignore if last button was pressed less than DELTA_T */
      if(get_time() > (last+DELTA_T))
      {
        /* generate token and send it to host */
        token();
        last = get_time();
      }
    }

    /* USB tasks */
    HID_Device_USBTask(&Keyboard_HID_Interface);
    USB_USBTask();
  }
}

/* configure hardware, init timer, setup LED, button... */
void SetupHardware()
{
  char init;

  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);

  /* reset eeprom */
  eeprom_read(ADDR_INIT, &init, 1);
  if(init != 42)
    clear_eeprom();

  /* init LED */
  LED_DDR |= (1 << LED);

  /* init button */
  BUTTON_DDR &= ~(1 << BUTTON);

  /*enable pullup*/
  PORTE |=(1<<BUTTON);

  /* init timer */
  timer_init();

  /* init seed */
  srand(get_seed());

  /* init nonce */
  update_nonce();

  /* Hardware Initialization */
  USB_Init();

  /* init i2c */
  #ifdef RTC
    rtc_init();
  #endif
}


/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
  HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);
  USB_Device_EnableSOFEvents();
}


/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
  static uint8_t success = 1;

  uint8_t bmRequestType = USB_ControlRequest.bmRequestType;
  uint8_t bRequest      = USB_ControlRequest.bRequest;
  //uint8_t wValue        = USB_ControlRequest.wValue;
  char data[51];

  HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);

  if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE))
  {
    char lock;
    uint16_t wLength = USB_ControlRequest.wLength;
    char pw[32];
    success = 1; /* default */

    eeprom_read(ADDR_LOCK, &lock, 1);
    if(lock == 0 || lock == 255)
      lock = 0;
    else
    {
      if(bRequest != OPENKUBUS_GET_NONCE && bRequest != OPENKUBUS_SET_TIMESTAMP && bRequest != OPENKUBUS_RESET)
      {
        success = 0;
        return;
      }
    }

    // read data
    if(wLength)
    {
      Endpoint_ClearSETUP();
      Endpoint_Read_Control_Stream_LE(data, MIN(sizeof(data), wLength));
      Endpoint_ClearIN();
    }

    switch(bRequest)
    {
      case OPENKUBUS_SET_LOCK:
        lock = 1;
        eeprom_write(ADDR_LOCK, &lock, 1);
        break;

      
      case OPENKUBUS_SET_OWNER:
        if(wLength)
          eeprom_write(ADDR_OWNER, data, wLength);
        else
          success = 0;
        
        break;
      

      case OPENKUBUS_SET_COMPANY:
        if(wLength)
          eeprom_write(ADDR_COMPANY, data, wLength);
        else
          success = 0;
        
        break;
      

      case OPENKUBUS_SET_DESCRIPTION:
        if(wLength)
          eeprom_write(ADDR_DESCRIPTION, data, wLength);
        else
          success = 0;
        
        break;


      case OPENKUBUS_SET_ID:
        if(wLength == 4)
          eeprom_write(ADDR_ID, data, wLength);
        else
          success = 0;
        
        break;


      case OPENKUBUS_SET_TIMESTAMP:
        if(wLength == 16)
        {
          aes256_ctx_t ctx;

          memset(&ctx, 0, sizeof(aes256_ctx_t));
          eeprom_read(ADDR_SEED, pw, sizeof(pw));

          aes256_init(pw, &ctx);
          aes256_dec(data, &ctx);

          if(strncmp(nonce, data, sizeof(nonce)) == 0)
          {
            set_timestamp(array2int((uint8_t *)&data[12]));
            update_nonce();
          }
          else
            success = 0;
        }
        else
          success = 0;

        break;


      case OPENKUBUS_RESET:
        if(wLength == 16)
        {
          aes256_ctx_t ctx;

          memset(&ctx, 0, sizeof(aes256_ctx_t));
          memcpy_P(pw, MASTER_PASSWORD, sizeof(pw));

          aes256_init(pw, &ctx);
          aes256_dec(data, &ctx);

          if(strncmp(nonce, data, sizeof(nonce)) == 0)
          {
            clear_eeprom();
            wdt_enable(WDTO_15MS);
            while(1);
          }
          else
            success = 0;
        }
        else
          success = 0;

        break;


      case OPENKUBUS_SET_SEED:
        if(wLength == LEN_SEED)
          eeprom_write(ADDR_SEED, data, LEN_SEED);
        else
          success = 0;

        break;


      case OPENKUBUS_SET_COUNTER:
        if(wLength == LEN_COUNTER)
          eeprom_write(ADDR_COUNTER, data, LEN_COUNTER);
        else
          success = 0;

        break;


      default:
        success = 0;
        break;
    }
  }
  else if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE))
  {
    uint8_t length = 0;
    uint8_t i;
    uint32_t temp32 = 0;
    char c;

    switch(bRequest)
    {
      case OPENKUBUS_GET_SUCCESS:
        data[length++] = success;
        break;


      case OPENKUBUS_GET_NONCE:
        for(i = 0; i < sizeof(nonce); i++)
          data[length++] = nonce[i];

        break;


      case OPENKUBUS_GET_TEMPERATURE:
        #ifdef RTC
          data[length++] = rtc_get_temperature();
        #else
          data[length++] = 0xFF;
        #endif
        break;


      case OPENKUBUS_GET_ID:
        for(i = 0; i < LEN_ID; i++)
        {
          eeprom_read(ADDR_ID+i, &c, 1); 
          data[length++] = c;
        }

        break;


      case OPENKUBUS_GET_TIME:
        temp32 = get_timestamp();

        data[length++] = temp32 >> 24;
        data[length++] = temp32 >> 16;
        data[length++] = temp32 >> 8;
        data[length++] = temp32;
        break;


      case OPENKUBUS_GET_SERIAL:
        for(i = 0x0e; i <= 0x18; i++)
          data[length++] = boot_signature_byte_get(i);
        break;


      case OPENKUBUS_GET_DESCRIPTION:
        for(i = 0; i < LEN_DESCRIPTION; i++)
        {
          eeprom_read(ADDR_DESCRIPTION+i, &c, 1); 
          data[length++] = c;
          if(c == 0)
            break;
        }

        break;


      case OPENKUBUS_GET_COMPANY:
        for(i = 0; i < LEN_COMPANY; i++)
        {
          eeprom_read(ADDR_COMPANY+i, &c, 1); 
          data[length++] = c;
          if(c == 0)
            break;
        }

        break;


      case OPENKUBUS_GET_OWNER:
        for(i = 0; i < LEN_OWNER; i++)
        {
          eeprom_read(ADDR_OWNER+i, &c, 1); 
          data[length++] = c;
          if(c == 0)
            break;
        }

        break;

      default:
        data[length++] = 0;
    }

    // send data
    Endpoint_ClearSETUP();
    Endpoint_Write_Control_Stream_LE(data, length);
    Endpoint_ClearOUT();
  }
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
  HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent
 *
 *  \return Boolean true to force the sending of the report, false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, uint8_t* const ReportID,
                                         const uint8_t ReportType, void* ReportData, uint16_t* const ReportSize)
{
  static uint8_t toggle = 0;
  uint8_t data, modifier;
  USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;

  if (toggle && fifo_get(&fifo, &data))
  {
    char2usb(&data, &modifier);
    KeyboardReport->KeyCode[0] = data;
    KeyboardReport->Modifier = modifier;
  }
  else
  {
    KeyboardReport->KeyCode[0] = 0;
    KeyboardReport->Modifier = 0;
  }

  *ReportSize = sizeof(USB_KeyboardReport_Data_t);

  toggle = !toggle;

  return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the created report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
}

void timer_init(void)
{
  // 8MHz / 256 = 31250Hz
  TCCR1B = (1 << CS12 | 1 << WGM12);

  // 31250Hz/625 = 50Hz
  // f = f_Clk/(N*(1+OCR1A))
  OCR1A  = 624; // see atmega16u4 datasheet

  TCNT1 = 0;

  // interrupt enable: ISR will be called once in 20ms
  TIMSK1 |= (1 << OCIE1A);
}


uint32_t get_time(void)
{
  uint32_t t;

  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    t = time;
  }

  return t;
}



ISR(TIMER1_COMPA_vect)
{
  time++;

  // every second...
  if((time % 50) == 0)
  {
    #ifndef RTC
    timestamp++;
    #endif

    // toggle led
    LED_PORT ^= (1 << LED);
  }
}

// generate token and send it to device
void token(void)
{
  aes256_ctx_t ctx;
  char out[32];
  char seed[LEN_SEED];
  char data[20];
  char counter[2];
  uint16_t i, n;
  uint32_t time;

  // read_into_ram:
  eeprom_read(ADDR_SEED, seed, LEN_SEED);

  // increase counter
  eeprom_read(ADDR_COUNTER, counter, 2);
  n = counter[0] + (counter[1] << 8);
  n++;

  counter[0] = (uint8_t) n;
  counter[1] = (uint8_t) (n >> 8);

  eeprom_write(ADDR_COUNTER, counter, 2);

  memset(&ctx, 0, sizeof(aes256_ctx_t));
  memset(out, 0, sizeof(out));

  data[0] = counter[0];
  data[1] = counter[1];

  for(i = 0; i < LEN_SEED-32; i++)
    data[i+2] = seed[32+i];


  time = get_timestamp();

  data[12] = time >> 24;
  data[13] = time >> 16;
  data[14] = time >>  8;
  data[15] = time;

  aes256_init(seed, &ctx);
  aes256_enc(data, &ctx);

  raw_to_base64(data, 16, out, sizeof(out));

  fifo_put(&fifo, 'z');
  fifo_puts(&fifo, (uint8_t*)out, strlen(out));
}

void char2usb(uint8_t *hex, uint8_t *modifier)
{
  *modifier = 0;

  switch(*hex)
  {
    case '\n': *hex = 0x28; break;
    case ' ': *hex = 0x2C; break;
    case '-': *hex = 0x2d; break;
    case '0': *hex = 0x27; break;
    case '/': *modifier = 1 << 5; *hex = 0x1E; break;
    case '=': *hex = 0x37; break; // = => 3
    case '+': *hex = 0x38; break; // = => 3

    default:
      if(*hex <= 'Z' && *hex >= 'A')
      {
        *modifier = 1 << 5; // right shift
        *hex = *hex - 65 + 0x04;
      }
      else if(*hex <= 'z' && *hex >= 'a')
        *hex = *hex - 97 + 0x04;
      else if(*hex >= '1' && *hex <= '9')
        *hex = *hex - 49 + 0x1E; // 49: '1'
  }
}

unsigned short get_seed(void)
{
  unsigned short seed = 0;
  unsigned short *p = (unsigned short*) (RAMEND+1);
  extern unsigned short __heap_start;
    
  while (p >= &__heap_start + 1)
    seed ^= * (--p);
    
  return seed;
}


void update_nonce(void)
{
  uint8_t i;

  for(i = 0; i < sizeof(nonce); i++)
    nonce[i] = rand();
}

void clear_eeprom(void)
{
  char c = 0;
  uint16_t addr;

  for(addr = 0; addr < 512; addr++)
    eeprom_write(addr, &c, 1);

  c = 42;
  eeprom_write(ADDR_INIT, &c, 1);
}
