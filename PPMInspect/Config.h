/*
  PPMInspect. A PPM signal analysis tool.

  MIT License

  Copyright (c) 2023 wlowi

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef _Config_h_
#define _Config_h_

#include "Arduino.h"
#include "TextUI.h"

#define PPMINSPECT_VERSION "0.9.17"

#define ENABLE_MEMDEBUG

#ifdef ENABLE_MEMDEBUG

/* ATMega 328 p
 *
 *    0 -   31 Register File (32)
 *   32 -   95 Standard IO Memory (64)
 *   96 -  255 Extended IO Memory (160)
 *  256 - 2303 SRAM (2048)
 */

/* Start of free memory in SRAM */
#define freeStart memdebug[0]
/* Stackpointer */
#define stackLow memdebug[1]
/* Gap size or initial free bytes */
#define gapSize memdebug[2]
/* Free bytes determined by free space check */
#define gapFree memdebug[3]

/* Initialize every byte within the gap between
 * freeStart and stackLow with bit pattern 01010101
 */
#define MEMDEBUG_INIT()                       \
do {                                          \
    uint16_t i;                               \
    cli();                                    \
    gapFree = 0;                              \
    freeStart = (size_t)malloc( 1);           \
    stackLow = SPH << 8 | SPL;                \
    gapSize = stackLow - freeStart - 2;       \
    for( i=0; i<gapSize; i++) {               \
        *(byte*)(freeStart+i) = 0x55;         \
    }                                         \
    sei();                                    \
} while( false)

/* Check how many bytes are still free.
 * (Unchanged pattern 01010101)
 */
#define MEMDEBUG_CHECK()                      \
do {                                          \
    uint16_t i;                               \
    gapFree = 0;                              \
    for( i=0; i<gapSize; i++) {               \
        if( *(byte*)(freeStart+i) != 0x55) {  \
            break;                            \
        } else {                              \
            gapFree++;                        \
        }                                     \
    }                                         \
} while( false)

#endif

/* ================================================================== */

/* PPM */

#define PPM_MAX_CHANNELS           16
#define PPM_MIN_CHANNELS            4

#define PPM_MAX_PULSE              (2*PPM_MAX_CHANNELS +2)

#define PORT_PPM_IN                8
#define PORT_ANALOG_IN             A3
#define PORT_VCC                   A0

/* Interrupt mask for PORT_PPM_IN */
#define PORT_PPM_IN_PCINT_MASK     ((uint8_t)_BV(PCINT10))

/* ================================================================== */

/* Voltage mutliplied by 10 to maintain 1 fractional digits.
 * 50 = 5.00V
 */
#define ADC_VOLTAGE                50
/* 10 bits => 1024 steps */
#define ADC_VCC_RESOLUTION       1024

/* Voltage divider values in 1000Ohm units.*/
#define ADC_VOLTAGE_DIVIDER_R1    100
#define ADC_VOLTAGE_DIVIDER_R2     47

/* ================================================================== */

#define BUTTON_COUNT                3
#define BUTTON_PORTS          5, 6, 7
/* Actions for short press */
#define BUTTON_SHORT_KEYS      KEY_UP,    KEY_ENTER, KEY_DOWN
/* Actions for long press ( > 300 mSec ) */
#define BUTTON_LONG_KEYS       KEY_RESET, KEY_CLEAR, KEY_NONE

/* ================================================================== */

/* Settings Default values */
#define PULSEVALIDMIN_usec        250
#define PULSEVALIDMAX_USEC        450
#define SERVOVALIDMIN_USEC        850
#define SERVOVALIDMAX_USEC       2150
#define SYNCVALIDMIN_usec        3000

/* ================================================================== */

typedef uint16_t checksum_t;

typedef struct config_t
{
    uint16_t pulseValidMin_usec;
    uint16_t pulseValidMax_usec;

    uint16_t servoValidMin_usec;
    uint16_t servoValidMax_usec;

    uint16_t syncValidMin_usec;

    int8_t   vppmAdjust;
    int8_t   vccAdjust;
    
    fixfloat1_t lowBattWarn;
    
    /* */

    checksum_t checksum;

} config_t;

#endif
