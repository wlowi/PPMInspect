/*
  PPMGenerate_ISR. A PPM generator for testing.

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

/*
 *      0  300     1100  1500  1900                22000
 *  ____     _______                 _____ ... ______
 *      |   |       |     |     |   |                |
 *      |___|       |     |     |___|                |
 *
 *      |   |
 *    PPM_SPACE
 *
 *      |---- PPM_MID ----|
 *
 *
 *  9 ch * 1.5 mSec = 13.5 mSec  SYNC = 22.0 - 13.5 = 6.5 mSec
 *
 */
 
#include <util/atomic.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define PPM_CHANNELS        9
#define PPM_PIN             2


typedef uint8_t  channel_t;
typedef uint16_t timingUsec_t;

#define PPM_FRAME_usec      ((timingUsec_t)22000)
#define PPM_SPACE_usec      ((timingUsec_t)  300)
#define PPM_MID_usec        ((timingUsec_t) 1200)
#define PPM_RANGE100_usec   ((timingUsec_t)  400)
/* 150 % */
#define PPM_RANGEMAX_usec   ((timingUsec_t)  600)

#define PPM_MIN_usec        (PPM_MID_usec - PPM_RANGEMAX_usec)
#define PPM_MAX_usec        (PPM_MID_usec + PPM_RANGEMAX_usec)

/* Initial high for sync */
#define PPM_INIT_usec       ((timingUsec_t) 8000)

typedef struct ppmSet_t {

  timingUsec_t channel[ PPM_CHANNELS ];

} ppmSet_t;

ppmSet_t ppmSet;

/* PPM generation state machine state.
 * Used in the interrupt routine.
 */
static uint8_t outputState;

/* inFrameTime is used to compute the remaining time until frame end */
static timingUsec_t inFrameTime_usec;

static channel_t outputChannel;

/* Convert timer low, high values to microseconds.
 * Division by 2 is because the timer runs in 0.5 usec resolution.
 */
#define CONVERT_TIMER_TO_USEC( l, h)  ((((timingUsec_t)h << 8) + l) >> 1)

/* PPM generation state machine states */
#define BEGIN_OF_FIRST_SPACE    0
#define END_OF_SPACE            1
#define END_OF_MARK             2
#define END_OF_LAST_SPACE       4

#define H( v ) (uint8_t)((v >> 8) & 0xff)
#define L( v ) (uint8_t)(v & 0xff)


/* For performance we direct access port registers.
 */
static volatile uint8_t *out;   // ptr to data output reg
static volatile uint8_t *mode;  // ptr to data direction reg
static uint8_t pinbit;          // pin bitmask

#define PIN_LOW()  *out &= ~pinbit
#define PIN_HIGH() *out |= pinbit


/* This compensates time from begin of ISR to actual timer reset */
#define RESET_ADJUST (byte)6

/* This compensates ISR service time */
#define ISR_ADJUST   6

ISR( TIMER1_COMPA_vect) {

  timingUsec_t nextTimerValue_usec;
  uint8_t h, l;

  l = TCNT1L;
  h = TCNT1H;

  inFrameTime_usec = CONVERT_TIMER_TO_USEC( l, h);

  switch ( outputState) {
    case END_OF_SPACE:
      PIN_HIGH();
      nextTimerValue_usec = ppmSet.channel[outputChannel];
 
      outputState = END_OF_MARK;
      break;

    case END_OF_MARK:
      PIN_LOW();
      nextTimerValue_usec = PPM_SPACE_usec;

      outputChannel++;
      if ( outputChannel < PPM_CHANNELS)  {
        outputState = END_OF_SPACE; // repeat for next channel
      } else {
        outputChannel = 0;
        outputState = END_OF_LAST_SPACE; // done
      }
      break;

    case BEGIN_OF_FIRST_SPACE:
      PIN_LOW();
      TCNT1H = (byte)0;
      TCNT1L = RESET_ADJUST;
      nextTimerValue_usec = PPM_SPACE_usec;
      inFrameTime_usec = 0;
      
      outputState = END_OF_SPACE;
      break;

    case END_OF_LAST_SPACE:
      PIN_HIGH();
      nextTimerValue_usec = PPM_FRAME_usec - inFrameTime_usec;

      outputState = BEGIN_OF_FIRST_SPACE;
      break;

    default:
      nextTimerValue_usec = 0;
  }

  nextTimerValue_usec += inFrameTime_usec;
  /* Multiply by 2 because the timer resulution is 0.5 micro sec. */
  nextTimerValue_usec <<= 1;
  nextTimerValue_usec -= ISR_ADJUST;

  /* Set output compare register. HIGH BYTE FIRST ! */
  OCR1AH = H( nextTimerValue_usec);
  OCR1AL = L( nextTimerValue_usec);
}

void setup() {

  initPPM();
}

void loop() {

  set_sleep_mode( SLEEP_MODE_IDLE);
  cli();
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
}

/* Initialization:
 *  - Set all channels to mid.
 *  - Switch PPM pin to output.
 *  - Initialize timer
 *      Timer 1 counts at a rate of 2Mhz ( 0.5 micro sec. )
 */
void initPPM() {

  ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {

    for ( channel_t i = 0; i < PPM_CHANNELS; i++) {
      ppmSet.channel[i] = PPM_MID_usec;
    }

    inFrameTime_usec = 0;
    outputChannel = 0;
    outputState = BEGIN_OF_FIRST_SPACE;

    /* Set port line to output */
    uint8_t port = digitalPinToPort(PPM_PIN);
    out = portOutputRegister(port);
    mode = portModeRegister(port);
    pinbit = digitalPinToBitMask(PPM_PIN);
  
    *mode |= pinbit; // set pin to output

    PIN_HIGH();
  
    /* Don't use output compare pins */
    TCCR1A = (byte)0;
    /* Prescaler /8 = 2Mhz = 0.5 usec */
    TCCR1B = _BV(CS11);
    /* Set initial timer counter value */
    TCNT1H = (byte)0;
    TCNT1L = (byte)0;
    /* Set output compare register. HIGH BYTE FIRST ! */
    OCR1AH = H( PPM_INIT_usec );
    OCR1AL = L( PPM_INIT_usec );
    /* Enable output compare match A interrupt */
    TIMSK1 = _BV(OCIE1A);
  }
}

/* Set timing for a channel.
 *  
 * "value" must be between -1000 and 1000 for a PPM range of 100% (1100 - 1900 usec)
 * Larger values are clipped to 150% (900 - 2100 usec).
 */
void setChannelValue(int channel, int value) {

  timingUsec_t t;

  /* Convert:

     value
     =======
     CHANNELVALUE_MID   0
     CHANNELVALUE_MIN   -1250 == -125%  == -500
     CHANNELVALUE_MAX    1250 ==  125%  ==  500

     To:

     ppmTiming_t
     ===========
     PPM_MID_usec        ((timingUsec_t) 1200)
     PPM_MIN_usec        (PPM_MID_usec - PPM_RANGE_usec)
     PPM_MAX_usec        (PPM_MID_usec + PPM_RANGE_usec)

  */

  if ( channel < PPM_CHANNELS) {

    t = PPM_MID_usec + (value * 2 / 5);

    if ( t < PPM_MIN_usec) t = PPM_MIN_usec;
    if ( t > PPM_MAX_usec) t = PPM_MAX_usec;

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {

      ppmSet.channel[ channel ] = t;

    }
  }
}
