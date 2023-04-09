/*
  PPMGenerate_PWM. A precise PPM generator for testing.

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
 * This Sketch is for an Arduino Nano
 * 
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
 *  9 ch * 1.5 mSec = 13.5 mSec  SYNC = 22.0 - 13.5 = 8.5 mSec
 *
 */

#include <util/atomic.h>
#include <avr/sleep.h>

#define PPM_CHANNELS        9

/* PWM output 
 *  Pin 9 on Arduino Nano (OC1A of timer 1)
 *  Pin 5 on Arduino Micro Pro (OC3A of timer3)
 */
#define PPM_PWM_PIN         9

typedef uint8_t  channel_t;
typedef uint16_t timingUsec_t;

#define PPM_FRAME_usec      ((timingUsec_t)22000)
#define PPM_SPACE_usec      ((timingUsec_t)  300)
#define PPM_MID_usec        ((timingUsec_t) 1500)

/* 100 % */
#define PPM_RANGE100_usec   ((timingUsec_t)  400)
/* 150 % */
#define PPM_RANGEMAX_usec   ((timingUsec_t)  600)

#define PPM_MIN_usec        (PPM_MID_usec - PPM_RANGEMAX_usec)
#define PPM_MAX_usec        (PPM_MID_usec + PPM_RANGEMAX_usec)

/* Initial high for sync */
#define PPM_INIT_usec       ((timingUsec_t) 8200)

typedef struct ppmSet_t {

  timingUsec_t channel[ PPM_CHANNELS ];

} ppmSet_t;

ppmSet_t ppmSet;

static channel_t outputChannel;
static timingUsec_t inFrameTime;

/* Timer 1 overflow interrupt */
ISR(TIMER1_OVF_vect) {

  timingUsec_t nextTimerValue;
  
  if( outputChannel >= PPM_CHANNELS) {
    
    nextTimerValue = (PPM_FRAME_usec << 1) - inFrameTime;
    inFrameTime = 0;
    outputChannel = 0;
    
  } else {
    
    nextTimerValue = ppmSet.channel[outputChannel] << 1;
    inFrameTime += nextTimerValue;
    outputChannel++;
  }

  /* We nedd to count 1498..1499..0..1
   * not 1498..1499..1500..0..1
   */
  nextTimerValue--;
  ICR1 = nextTimerValue;
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

void initPPM() {

  ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {

    for ( channel_t i = 0; i < PPM_CHANNELS; i++) {
      ppmSet.channel[i] = PPM_MID_usec;
    }

    outputChannel = 0;
    inFrameTime = 0;
    
    pinMode( PPM_PWM_PIN, OUTPUT);
    
    /* Enable timer in power reduction register */
    PRR &= ~bit(PRTIM1);
    
    /* Set initial timer counter value */
    TCNT1 = 0;
       
    /* COM1A1, COM1A0     : Set OC1A on compare match, clear at TOP
     * WGM13, WGM12, WGM11: TOP is ICR1
     * CS11               : Prescaler /8 = 2Mhz = 0.5 usec
     */
    TCCR1A = bit(COM1A1) | bit(COM1A0) | bit(WGM11);
    TCCR1B = bit(WGM13) | bit(WGM12) | bit(CS11);
    TCCR1C = (byte)0;

    /* Set TOP */
    ICR1 = ((PPM_SPACE_usec + PPM_INIT_usec) << 1) -1;

    /* Set compare to end of pulse time.
     * Subtract 1 because we count 298..299..0..1
     */
    OCR1A = (PPM_SPACE_usec << 1) -1;
    
    /* TOIE1: Timer overflow interrupt */
    TIMSK1 = bit(TOIE1);
  }
}

/*
 * Set PPM timing for a channel.
 * 
 * ch - channel        [0, PPM_CHANNELS-1]
 * t  - timing in usec [PPM_MIN_usec, PPM_MAX_usec]
 */
void  setPPM( channel_t ch, timingUsec_t t)
{
    if( ch < PPM_CHANNELS) {

        if( t < PPM_MIN_usec) t = PPM_MIN_usec;
        if( t > PPM_MAX_usec) t = PPM_MAX_usec;

        ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
            ppmSet.channel[ch] = t;
        }
    }
}
