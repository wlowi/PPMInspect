/*
  PPMGenerate_DELAY. A PPM generator for testing.

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
 
const byte PPM_PIN = 2;

const byte CHANNELS = 9;
const int  POS_usec = 1500;
const int  SPACE_usec = 300;
const int  MARK_usec = POS_usec - SPACE_usec;
const int  FRAME_usec = 22000;
const int  SYNC_usec = FRAME_usec - (CHANNELS * POS_usec) - SPACE_usec;

volatile uint8_t *out;
volatile uint8_t *mode;
uint8_t pinbit;

#define SET_LOW()  *out &= ~pinbit
#define SET_HIGH() *out |= pinbit

void setup() {

  uint8_t port = digitalPinToPort(PPM_PIN);
  out = portOutputRegister(port);
  mode = portModeRegister(port);
  pinbit = digitalPinToBitMask(PPM_PIN);
  
  *mode |= pinbit; // set to output
}

void loop() {

  cli();
  
  for( byte ch = 0; ch < CHANNELS; ch ++) {
    SET_LOW();
    delayMicroseconds( SPACE_usec);
    SET_HIGH();
    delayMicroseconds( MARK_usec);
  }

  SET_LOW();
  delayMicroseconds( SPACE_usec);
  SET_HIGH();
  delayMicroseconds( SYNC_usec);

  sei();
}
