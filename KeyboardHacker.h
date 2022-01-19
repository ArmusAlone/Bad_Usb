//Mudified by ARMUS


#ifndef __KeyboardHacker_h__
#define __KeyboardHacker_h__

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>

#include "usbdrv.h"
#include "scancode-ascii-table.h"



typedef uint8_t byte;


#define BUFFER_SIZE 2 


static uchar    idleRate;          


const PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
  0x05, 0x01,                    
  0x09, 0x06,
  0xa1, 0x01,        
  0x05, 0x07,      
  0x19, 0xe0,                    
  0x29, 0xe7,                    
  0x15, 0x00,     
  0x25, 0x01,     
  0x75, 0x01, 
  0x95, 0x08,  
  0x81, 0x02,      
  0x95, 0x01,           
  0x75, 0x08, 
  0x25, 0x65,       
  0x19, 0x00,                    
  0x29, 0x65,                    
  0x81, 0x00,      
  0xc0     
};



/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */

#define CONTROL_L    (1<<0)
#define SHIFT_L      (1<<1)
#define ALT_L        (1<<2)
#define OS_L         (1<<3)
#define CONTROL_R    (1<<4)
#define SHIFT_R      (1<<5)
#define ALT_R        (1<<6)
#define Menu          118
#define _A        4
#define _B        5
#define _C        6
#define _D        7
#define _E        8
#define _F        9
#define _G        10
#define _H        11
#define _I        12
#define _J        13
#define _K        14
#define _L        15
#define _M        16
#define _N        17
#define _O        18
#define _P        19
#define _Q        20
#define _R        21
#define _S        22
#define _T        23
#define _U        24
#define _V        25
#define _W        26
#define _X        27
#define _Y        28
#define _Z        29
#define _1        30
#define _2        31
#define _3        32
#define _4        33
#define _5        34
#define _6        35
#define _7        36
#define _8        37
#define _9        38
#define _0        39

#define Enter     40
#define Space     44

#define F1        58
#define F2        59
#define F3        60
#define F4        61
#define F5        62
#define F6        63
#define F7        64
#define F8        65
#define F9        66
#define F10       67
#define F11       68
#define F12       69
#define Esc       41
#define Tab       43
#define Backspace 42
#define Slash     46
#define Dele      76
#define Backslash 84
#define Star      85
#define Plus      87
#define Dot       99
#define Minus     86
#define UP        82
#define DOWN      81
#define LEFT      80
#define RIGHT     79
//for android
#define Apps      4,  (1<<2)
#define Home      40, (1<<3)
#define Back      42, (1<<3)
#define Recent    43, (1<<3)
#define Lock      15, (1<<3)    
#define Browser   5,  (1<<3)
#define Massages  22, (1<<3)
//shortcut for windows 
#define SAVE      22, (1<<0)
#define Run       21, (1<<3)
#define Lock      15, (1<<3)
#define File      8,  (1<<3)




class HIDDevice : public Print {
 public:
  HIDDevice () {
    cli();
    usbDeviceDisconnect();
    _delay_ms(250);
    usbDeviceConnect();


    usbInit();
      
    sei();

    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
    
  void update() {
    usbPoll();
  }
	
	void delay(long milli) {
		unsigned long last = millis();
	  while (milli > 0) {
	    unsigned long now = millis();
	    milli -= now - last;
	    last = now;
	    update();
	  }
	}
  
  void sendKey(byte keyStroke) {
    sendKey(keyStroke, 0);
  }

  void sendKey(byte keyStroke, byte modifiers) {
	sendKeyPress(keyStroke, modifiers);
	sendKeyPress(0,0);
  }

  void sendKeyPress(byte keyPress) {
	sendKeyPress(keyPress, 0);
  }

  void sendKeyPress(byte keyPress, byte modifiers) {
   	while (!usbInterruptIsReady()) {

    	usbPoll();
    	_delay_ms(5);
    }
    
    memset(reportBuffer, 0, sizeof(reportBuffer));
		
    reportBuffer[0] = modifiers;
    reportBuffer[1] = keyPress;
    
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
  
  size_t write(uint8_t chr) {
    uint8_t data = pgm_read_byte_near(ascii_to_scan_code_table + (chr - 8));
    sendKey(data & 0b01111111, data >> 7 ? SHIFT_R : 0);
    return 1;
  }
    
  uchar    reportBuffer[2];  
  using Print::write;
};

HIDDevice HID = HIDDevice();

#ifdef __cplusplus
extern "C"{
#endif 
	uchar usbFunctionSetup(uchar data[8]) {
    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    usbMsgPtr = HID.reportBuffer; 
    if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {

      if (rq->bRequest == USBRQ_HID_GET_REPORT) {
			  
				return 0;

      } else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
				
				return 0;
				
      } else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
				idleRate = rq->wValue.bytes[1];
				
      }
    } else {
      
    }
		
    return 0;
  }
#ifdef __cplusplus
} 
#endif


#endif
