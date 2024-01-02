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

#ifndef _PPM_h_
#define _PPM_h_

#include "Config.h"
#include "TextUI.h"

typedef struct ppm_t {
  
    bool     sync;
    uint8_t  channels;
    uint16_t frameMin_usec;
    uint16_t frameMax_usec;
    uint16_t pulseMin_usec;
    uint16_t pulseMax_usec;
    fixfloat1_t vLevel_low;
    fixfloat2_t vLevel_high;
    bool     pulseLevel;
    uint32_t frames;
    uint16_t badFrames;
    uint16_t badCount;
    uint16_t badPulse;
    uint16_t channel_usec[PPM_MAX_CHANNELS];
    uint16_t channelMin_usec[PPM_MAX_CHANNELS];
    uint16_t channelMax_usec[PPM_MAX_CHANNELS];
} ppm_t;

#define PWM_HISTORY 10

typedef struct pwm_t {
  
    bool     sync;
    uint32_t frameMin_usec;
    uint32_t frameMax_usec;
    fixfloat2_t freq;
    uint32_t frames;
    uint32_t miss;

    int8_t   lastUsed;
    uint16_t pulseL_usec[PWM_HISTORY];
    uint16_t pulseH_usec[PWM_HISTORY];
} pwm_t;

class PPM {

    private:
        /* Three sets of data. 
         * ppm[0] may be inconsistent and is written by the ISR.
         * ppm[1] and ppm[2] are always consistent.
         * one set is exported to the data viewer while the other
         * set is exchanged with ppm[0] when the set is complete and consistent.
         */
        ppm_t ppm[3];
        pwm_t pwm[3];

        uint8_t writeSet = 0;
        uint8_t stableSet = 1;
        uint8_t exportSet = 2;

    public:
        void countChannels( ppm_t *wSet, uint16_t time_usec, bool level);
        void initTimings( ppm_t *wSet);
        boolean storeFrame( ppm_t *wSet, uint16_t time_usec, bool level);
        void storePulse( ppm_t *wSet, uint16_t time_usec);
        void storeFrameTime( ppm_t *wSet, uint16_t frame_usec);
        void storeServoTime( ppm_t *wSet, uint16_t servo_usec);
        
        void startADC( uint8_t convertType);
        fixfloat1_t analogConvert( uint8_t convertType, uint16_t v) const;
        
        fixfloat1_t readADC();
        fixfloat1_t readVCC();
        
        void startPPMScan();
        ppm_t *getPPM();

        void startPWMScan();
        pwm_t *getPWM();

        void stopScan();

        /* The following four methods should only get called from ISR */
        ppm_t *getPPMWriteSet();
        void switchPPMWriteSet();

        pwm_t *getPWMWriteSet();
        void switchPWMWriteSet();

        boolean fetchArray( uint8_t dataArray[], uint8_t sz, uint16_t resUsec, uint8_t oversampling, uint8_t triggerMode, fixfloat1_t triggerLevel,  uint16_t triggerDelay);
};

#endif
