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

#include "PPM.h"

#ifdef ARDUINO
#include <util/atomic.h>
#include <avr/sleep.h>
#endif

PPM ppm;

/* Config */
extern config_t settings;

/* Detect PPM steps */
#define DETECT_STEP_INIT         0
#define DETECT_STEP_SYNCWAIT     1
#define DETECT_STEP_CHANNELWAIT  2
#define DETECT_STEP_VERIFY       3
#define DETECT_STEP_SYNCED       4

/* Detect PWM only */
#define DETECT_STEP_PWM          5

uint8_t detectStep;
uint8_t channels;
uint8_t detectedChannels;
uint8_t pulseIdx;
uint16_t pulse_usec[PPM_MAX_PULSE];
uint16_t lastCount;

#define ADC_IDLE  0
/* ADC started from PPM scan ISR */
#define ADC_PPM   1
/* ADC started for Voltmeter */
#define ADC_VM    2
/* ADC started for Vcc (Battery) */
#define ADC_VCC   3

volatile uint8_t adcConvertType = ADC_IDLE;
volatile uint16_t adcValue;


/*
 * ADC conversion complete interrupt.
 */
ISR(ADC_vect) {

    uint16_t v;

    /* MUST read ADCL first */
    v = ADCL;
    v |= (ADCH << 8);

    if (adcConvertType == ADC_PPM) {

        ppm_t* wSet = ppm.getPPMWriteSet();
        fixfloat1_t newVcc = ppm.analogConvert(ADC_PPM, v);

        if (newVcc < 20) {
            wSet->vLevel_low = newVcc;
        }
        else {
            wSet->vLevel_high = newVcc;
        }

    }
    else {

        adcValue = v;
    }
}

ISR(TIMER1_OVF_vect) {

    if (detectStep == DETECT_STEP_PWM) {
        pwm_t* pwmWSet = ppm.getPWMWriteSet();

        pwmWSet->sync = false;
        pwmWSet->miss++;

        ppm.switchPWMWriteSet();
    }
    else {
        ppm_t* ppmWSet = ppm.getPPMWriteSet();

        ppmWSet->channels = 0;
        ppmWSet->sync = false;

        if (detectStep != DETECT_STEP_INIT) {
            detectStep = DETECT_STEP_INIT;
            ppmWSet->badFrames++;
        }

        ppm.switchPPMWriteSet();
    }
}

ISR(TIMER1_CAPT_vect) {

    ppm_t* ppmWSet;
    pwm_t* pwmWSet;

    uint8_t h, l;
    uint16_t time_usec;
    bool level;

    l = ICR1L;
    h = ICR1H;

    /* Flip detection edge */
    TCCR1B ^= bit(ICES1);
    TIFR1 |= bit(ICF1);

    time_usec = (((uint16_t)h << 8) | l) - lastCount;
    lastCount = (((uint16_t)h << 8) | l);
    time_usec >>= 1;  /* Divide by 2 because of 0.5 usec timer resolution */

    level = digitalRead(PORT_PPM_IN);

    if( detectStep != DETECT_STEP_PWM) {
        ppmWSet = ppm.getPPMWriteSet();
        ppm.startADC(ADC_PPM);
    } else {
        TCNT1 = 0;
        lastCount = 0;
        pwmWSet = ppm.getPWMWriteSet();
    }

    if (TIFR1 & bit(TOV1)) // timer overflow
    {
        /* This should actually never happen as we have overflow interrupt enabled */
        TIFR1 |= bit(TOV1); /* clear overflow bit */
        if( detectStep != DETECT_STEP_PWM) {
            detectStep = DETECT_STEP_INIT;
        }
    }
    else {
        switch (detectStep) {
        case DETECT_STEP_INIT:
            TCNT1 = 0;
            lastCount = 0;

            pulseIdx = 0;
            ppmWSet->channels = 0;
            ppmWSet->sync = false;
            ppm.switchPPMWriteSet();
            detectStep = DETECT_STEP_SYNCWAIT;
            break;

        case DETECT_STEP_SYNCWAIT:
            if (time_usec > settings.syncValidMin_usec) {
                TCNT1 = 0;
                lastCount = 0;

                ppmWSet->pulseLevel = level;
                channels = 0;
                detectedChannels = 0;
                detectStep = DETECT_STEP_CHANNELWAIT;
            }
            break;

        case DETECT_STEP_CHANNELWAIT:
            ppm.countChannels(ppmWSet, time_usec, level);
            break;

        case DETECT_STEP_VERIFY:
        case DETECT_STEP_SYNCED:
            if (ppm.storeFrame(ppmWSet, time_usec, level)) {

                /* We don't set the timer back to 0.
                 * Instead we get the current timer and compute the offset time
                 * used by the ISR by subtracting the counter stored in lastCount.
                 * This is the corrected new timer base.
                 */
                l = TCNT1L;
                h = TCNT1H;
                time_usec = (((uint16_t)h << 8) | l) - lastCount + 2;

                TCNT1 = time_usec;

                lastCount = 0;
            }

            break;

        /********** PWM scan ***********/

        case DETECT_STEP_PWM:
            if( level) {
                pwmWSet->pulseL_usec = time_usec;
                if( pwmWSet->pulseH_usec > 0) {
                    uint32_t fTime = (uint32_t)time_usec + pwmWSet->pulseH_usec;
                    if( pwmWSet->frameMax_usec == 0L) {
                        pwmWSet->frameMax_usec = 1L; // skip first 
                    } else if( fTime > pwmWSet->frameMax_usec) {
                        pwmWSet->frameMax_usec = fTime;
                    }
                    if( pwmWSet->frameMin_usec == 0L) {
                        pwmWSet->frameMin_usec = UINT32_MAX; // skip first
                    } else if ( fTime < pwmWSet->frameMin_usec) {
                        pwmWSet->frameMin_usec = fTime;
                    }

                    pwmWSet->freq = (100000000L / fTime); // 2 digits right of dot
                    pwmWSet->frames++;
                    pwmWSet->sync = true;
                }
            } else {
                pwmWSet->pulseH_usec = time_usec;
            }
            ppm.switchPWMWriteSet();
            break;
        }
    }
}

void PPM::countChannels(ppm_t* wSet, uint16_t time_usec, bool level) {

    uint16_t servo_usec;

    if (level == wSet->pulseLevel) { // previous was not a pulse

        if (time_usec > settings.syncValidMin_usec) { // Sync detected

            if (channels >= PPM_MIN_CHANNELS) {
                TCNT1 = 0;
                lastCount = 0;
                detectedChannels = channels;
                channels = 0;
                pulseIdx = 0;
                detectStep = DETECT_STEP_VERIFY;
            }
            else {
                detectStep = DETECT_STEP_INIT;
            }

        }
        else {
            if (pulseIdx > 0) {
                servo_usec = time_usec + pulse_usec[pulseIdx - 1];
                /* Check valid servo timing */
                if (servo_usec >= settings.servoValidMin_usec && servo_usec <= settings.servoValidMax_usec) {
                    channels++;
                }
                else {
                    detectStep = DETECT_STEP_INIT;
                }
            }
            else {
                detectStep = DETECT_STEP_INIT;
            }
        }

    }
    else { // was a pulse
        if (time_usec < settings.pulseValidMin_usec || time_usec > settings.pulseValidMax_usec) {
            detectStep = DETECT_STEP_INIT;
        }
    }

    storePulse(wSet, time_usec);
}

boolean PPM::storeFrame(ppm_t* wSet, uint16_t time_usec, bool level) {

    uint16_t servo_usec;

    if (level == wSet->pulseLevel) { // previous was not a pulse

        if (time_usec > settings.syncValidMin_usec) { // Sync detected

            if (channels == detectedChannels) { // All channels scanned

                if (detectStep == DETECT_STEP_SYNCED) {
                    storeFrameTime(wSet, lastCount >> 1);

                    wSet->channels = detectedChannels;
                    wSet->frames++;
                    wSet->sync = true;
                    switchPPMWriteSet();

                }
                else { // DETECT_STEP_VERIFY => DETECT_STEP_SYNCED
                    initTimings(wSet);
                    detectStep = DETECT_STEP_SYNCED;
                }

                channels = 0;
                pulseIdx = 0;
                return true;

            }
            else { // servo channels missing
                if (detectStep == DETECT_STEP_SYNCED) {
                    wSet->badCount++;
                }
                detectStep = DETECT_STEP_INIT;
            }

        }
        else { // No sync

            if (pulseIdx > 0) {
                servo_usec = time_usec + pulse_usec[pulseIdx - 1];
                /* Check valid servo timing */
                if (servo_usec >= settings.servoValidMin_usec && servo_usec <= settings.servoValidMax_usec) {
                    if (detectStep == DETECT_STEP_SYNCED) {
                        storeServoTime(wSet, servo_usec);
                    }
                    channels++;
                }
                else {
                    detectStep = DETECT_STEP_INIT;
                }

            }
            else {
                detectStep = DETECT_STEP_INIT;
            }
        }

    }
    else { // was a pulse
        if (detectStep == DETECT_STEP_SYNCED) {
            if (time_usec < wSet->pulseMin_usec) {
                wSet->pulseMin_usec = time_usec;
            }
            if (time_usec > wSet->pulseMax_usec) {
                wSet->pulseMax_usec = time_usec;
            }

            if (time_usec < settings.pulseValidMin_usec || time_usec > settings.pulseValidMax_usec) {
                wSet->badPulse++;
            }
        }
    }

    storePulse(wSet, time_usec);

    return false;
}

void PPM::initTimings(ppm_t* wSet) {

    wSet->frameMin_usec = wSet->pulseMin_usec = UINT16_MAX;
    wSet->frameMax_usec = wSet->pulseMax_usec = 0;

    for (uint8_t ch = 0; ch < detectedChannels; ch++) {
        wSet->channel_usec[ch] = 0;
        wSet->channelMin_usec[ch] = UINT16_MAX;
        wSet->channelMax_usec[ch] = 0;
    }
}

void PPM::storePulse(ppm_t* wSet, uint16_t time_usec) {

    if (pulseIdx < PPM_MAX_PULSE) {
        pulse_usec[pulseIdx] = time_usec;
        pulseIdx++;
    }
    else {
        detectStep = DETECT_STEP_INIT;
    }
}

void PPM::storeFrameTime(ppm_t* wSet, uint16_t frame_usec) {

    if (frame_usec < wSet->frameMin_usec) {
        wSet->frameMin_usec = frame_usec;
    }
    if (frame_usec > wSet->frameMax_usec) {
        wSet->frameMax_usec = frame_usec;
    }
}

void PPM::storeServoTime(ppm_t* wSet, uint16_t servo_usec) {

    wSet->channel_usec[channels] = servo_usec;
    if (servo_usec < wSet->channelMin_usec[channels]) {
        wSet->channelMin_usec[channels] = servo_usec;
    }
    if (servo_usec > wSet->channelMax_usec[channels]) {
        wSet->channelMax_usec[channels] = servo_usec;
    }
}

void PPM::startPPMScan() {

    ATOMIC_BLOCK(ATOMIC_FORCEON) {

        pulseIdx = 0;
        memset(&ppm[0], 0, 3 * sizeof(ppm_t));
        detectStep = DETECT_STEP_INIT;

        pinMode(PORT_PPM_IN, INPUT);
        /* disable pull-up */
        digitalWrite(PORT_PPM_IN, LOW);

        /* ENABLE TIMER INPUT CAPTURE */
        /* COUNTER 1 */

        /* Don't use output compare pins, normal mode */
        TCCR1A = (byte)0;

        /* Enable Input Capture Noice Canceler
         * Prescaler /8 = 2Mhz = 0.5 usec
         */
        TCCR1B = bit(ICNC1) | bit(CS11);

        TCNT1 = 0;
        lastCount = 0;

        /* Enable timer overflow interrupt
         * Enable input capture interrupt
         */
        TIFR1 |= bit(TOV1); /* clear overflow bit */
        TIMSK1 |= bit(ICIE1) | bit(TOIE1);
    }
}

void PPM::stopScan() {

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        TIMSK1 &= ~(bit(ICIE1) | bit(TOIE1));
    }
}

ppm_t* PPM::getPPM() {

    uint8_t tmp;

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        tmp = exportSet;
        exportSet = stableSet;
        stableSet = tmp;
    }

    return &(ppm[exportSet]);
}

ppm_t* PPM::getPPMWriteSet() {

    return &(ppm[writeSet]);
}

void PPM::switchPPMWriteSet() {

    memcpy(&(ppm[stableSet]), &(ppm[writeSet]), sizeof(ppm_t));
}

/********* PWM Scan **********/

void PPM::startPWMScan() {

    ATOMIC_BLOCK(ATOMIC_FORCEON) {

        pulseIdx = 0;
        memset(&pwm[0], 0, 3 * sizeof(pwm_t));
        detectStep = DETECT_STEP_PWM;

        pinMode(PORT_PPM_IN, INPUT);
        /* disable pull-up */
        digitalWrite(PORT_PPM_IN, LOW);

        /* ENABLE TIMER INPUT CAPTURE */
        /* COUNTER 1 */

        /* Don't use output compare pins, normal mode */
        TCCR1A = (byte)0;

        /* Enable Input Capture Noice Canceler
         * Prescaler /8 = 2Mhz = 0.5 usec
         */
        TCCR1B = bit(ICNC1) | bit(CS11);

        TCNT1 = 0;
        lastCount = 0;

        /* Enable timer overflow interrupt
         * Enable input capture interrupt
         */
        TIFR1 |= bit(TOV1); /* clear overflow bit */
        TIMSK1 |= bit(ICIE1) | bit(TOIE1);
    }
}

pwm_t* PPM::getPWM() {

    uint8_t tmp;

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        tmp = exportSet;
        exportSet = stableSet;
        stableSet = tmp;
    }

    return &(pwm[exportSet]);
}

pwm_t* PPM::getPWMWriteSet() {

    return &(pwm[writeSet]);
}

void PPM::switchPWMWriteSet() {

    memcpy(&(pwm[stableSet]), &(pwm[writeSet]), sizeof(pwm_t));
}

/***************/

void PPM::startADC(uint8_t convertType) {

    uint8_t port;

    /* Skip this conversion if started from PPM Scan and ADC is not idle,
     * We don't want to block the ISR.
     */
    if (convertType == ADC_PPM && adcConvertType != ADC_IDLE) { return; }

    if (convertType == ADC_VCC) {
        port = PORT_VCC;
    }
    else {
        port = PORT_ANALOG_IN;
    }

    while (adcConvertType != ADC_IDLE);
    adcConvertType = convertType;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

        /* Disable power reduction for ADC */
        PRR &= ~bit(PRADC);

        /* REFS1 = 0, REFS0 = 1   ==>   VCC with ext. cap. on AREF */
        ADMUX = bit(REFS0);

        /* Prescaler /128   ==>   16MHz / 128 = 125KHz */
        ADCSRA = bit(ADEN) | bit(ADPS2) | bit(ADPS1) | bit(ADPS0);
        ADCSRB = 0;

        /* Set MUX */
        ADMUX &= ~(bit(MUX3) | bit(MUX2) | bit(MUX1) | bit(MUX0));
        ADMUX |= (port - A0);

        /* Start convertsion */
        ADCSRA |= bit(ADSC) | bit(ADIE);
    }
}

fixfloat1_t PPM::readADC() {

    startADC(ADC_VM);
    /* Wait for completion */
    while (ADCSRA & bit(ADSC));

    return analogConvert(ADC_VM, adcValue);
}

fixfloat1_t PPM::readVCC() {

    startADC(ADC_VCC);
    /* Wait for completion */
    while (ADCSRA & bit(ADSC));

    return analogConvert(ADC_VCC, adcValue);
}

fixfloat1_t PPM::analogConvert(uint8_t convertType, uint16_t rawADCvalue) const {

    long vin = rawADCvalue;

    if (convertType == ADC_VCC) {
        vin = vin * 1000 / (1000 + settings.vccAdjust);
    }
    else /* ADC_PPM or ADC_VM */ {
        vin = vin * 1000 / (1000 + settings.vppmAdjust);
    }

    vin = vin * ADC_VOLTAGE * (ADC_VOLTAGE_DIVIDER_R1 + ADC_VOLTAGE_DIVIDER_R2) / ADC_VOLTAGE_DIVIDER_R2 / ADC_VCC_RESOLUTION;
    adcConvertType = ADC_IDLE;

    return (fixfloat1_t)vin;
}

/*
 * triggerMode 0 = free running 1 = raising edge, 2 = falling edge
 */
boolean PPM::fetchArray(uint8_t dataArray[], uint8_t sz, uint16_t resUsec, uint8_t oversampling, uint8_t triggerMode, fixfloat1_t triggerLevel, uint16_t triggerDelay) {

    uint8_t p, v, o;
    uint8_t level;
    bool hit = false;
    uint8_t sample = 0;
    uint16_t v16;

    uint8_t resUsecH;
    uint8_t resUsecL;


/* ***************** */
/* A few handy macros make the code way more readable */

#define START_ADC_CONVERSION() \
do { \
  ADCSRA |= bit(ADSC); \
} while (false)

#define WAIT_ADC_COMPLETE() \
do { \
  while( ADCSRA & bit(ADSC)) { } \
} while (false)

#define WAIT_TIMER_OVERFLOW() \
do { \
  TIFR1 |= bit(TOV1); \
  while( (TIFR1 & bit(TOV1)) == 0) { } \
} while (false)

#define CLEAR_TIMER_OVERFLOW() \
do { \
  TIFR1 |= bit(TOV1); \
} while (false)

#define SET_TIMER_TOPA( h, l) \
do { \
  OCR1AH = (byte)(h); \
  OCR1AL = (byte)(l); \
} while (false)

#define SET_TIMER_TOPB( h, l) \
do { \
  OCR1BH = (byte)(h); \
  OCR1BL = (byte)(l); \
} while (false)

#define RESET_TIMER() \
do { \
  TCNT1H = (byte)0; \
  TCNT1L = (byte)0; \
} while (false)

#define NO_TIMER_INTERRUPT ((TIFR1 & bit(TOV1)) == 0)

/* ***************** */

    if (oversampling) {
        if (resUsec < 50) {
            oversampling = 0;
        }
        else if (resUsec < 100) {
            oversampling = 2;
        }
        else if (resUsec < 200) {
            oversampling = 4;
        }
        else {
            oversampling = 8;
        }
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

        /* Disable power reduction for ADC */
        PRR &= ~bit(PRADC);

        /* REFS1 = 0, REFS0 = 1   ==>   VCC with ext. cap. on AREF */
        ADMUX = bit(REFS0);

        /* Prescaler /2   ==>   16MHz / 2 = 8000KHz */
        ADCSRA = bit(ADEN) | bit(ADPS0);
        ADCSRB = 0;

        /* Set mux and left adjusst result */
        ADMUX &= ~(bit(MUX3) | bit(MUX2) | bit(MUX1) | bit(MUX0));
        ADMUX |= bit(ADLAR) | (PORT_ANALOG_IN - A0);

        DIDR0 |= bit(PORT_ANALOG_IN - A0);


        /* Fast PWM mode */
        TCCR1A = bit(WGM11) | bit(WGM10);

        /* Prescaler /8 = 2Mhz = 0.5 usec */
        TCCR1B = bit(WGM13) | bit(WGM12) | bit(CS11);

        TCCR1C = (byte)0;

        SET_TIMER_TOPA(0, 0);
        SET_TIMER_TOPB(0, 0);


        resUsec <<= 1; // *2 because of 0.5 usec timer resolution

        if (oversampling) {
            resUsec /= oversampling;
        }

        resUsecH = resUsec >> 8;
        resUsecL = resUsec & 0xff;


        switch (triggerMode) {

        case 1: // raising edge
        case 2: // falling edge

            level = (byte)(triggerLevel * 5 / 3);

            if (triggerDelay) {

                triggerDelay <<= 1; // *2 because of 0.5 usec timer resolution

                RESET_TIMER();
                SET_TIMER_TOPA(triggerDelay >> 8, triggerDelay);
                WAIT_TIMER_OVERFLOW();

                /* Preload new TOP. Becomes active at timer overflow */
                SET_TIMER_TOPA(resUsecH, resUsecL);

            }
            else {

                RESET_TIMER();
                SET_TIMER_TOPA(resUsecH, resUsecL);
                WAIT_TIMER_OVERFLOW();
            }

            START_ADC_CONVERSION();
            WAIT_ADC_COMPLETE();
            p = ADCH;

            CLEAR_TIMER_OVERFLOW();

            for (uint16_t w = 0; w < 25000; w++) {

                START_ADC_CONVERSION();
                WAIT_ADC_COMPLETE();
                v = ADCH;

                if (((triggerMode == 1) && p < level && v > level)
                    || ((triggerMode == 2) && p > level && v < level)) {
                    if (triggerDelay && NO_TIMER_INTERRUPT) {

                        RESET_TIMER();
                        CLEAR_TIMER_OVERFLOW();

                    }
                    else {

                        RESET_TIMER();
                        WAIT_TIMER_OVERFLOW();

                        dataArray[sample++] = p;
                        dataArray[sample++] = v;
                        hit = true;
                        break;
                    }
                }

                p = v;
            }
            break;

        case 0: // free running
        default:

            RESET_TIMER();
            SET_TIMER_TOPA(resUsecH, resUsecL);
            WAIT_TIMER_OVERFLOW();

            hit = true;
            break;
        }

        if (!hit) { return false; }

        START_ADC_CONVERSION();

        if (oversampling) {

            for (; sample < sz; ) {

                v16 = 0;

                for (o = 0; o < oversampling; o++) {

                    WAIT_ADC_COMPLETE();

                    /* result is left adjusted and we want highest 8 bits only */
                    v16 += ADCH;

                    WAIT_TIMER_OVERFLOW();
                    START_ADC_CONVERSION();
                }

                if (oversampling == 8) {
                    v16 >>= 3;
                }
                else if (oversampling == 4) {
                    v16 >>= 2;
                }
                else if (oversampling == 2) {
                    v16 >>= 1;
                }

                dataArray[sample++] = (byte)v16;
            }

        }
        else {

            for (; sample < sz; ) {

                WAIT_ADC_COMPLETE();

                /* result is left adjusted and we want highest 8 bits only */
                dataArray[sample++] = ADCH;

                WAIT_TIMER_OVERFLOW();
                START_ADC_CONVERSION();
            }
        }
    }

    return true;
}
