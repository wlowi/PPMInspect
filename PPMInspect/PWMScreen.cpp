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

#include "PWMScreen.h"
#include "ScopeScreen.h"

extern ScopeScreen scopeScreen;

/* Config */
extern config_t settings;

#define ROW_COUNT 8

const char s1[] PROGMEM = "PWM";
const char s2[] PROGMEM = "Cyc.";
const char s3[] PROGMEM = "Pulse L";
const char s4[] PROGMEM = "Pulse H";
const char s5[] PROGMEM = "Duty Cyc.";
const char s6[] PROGMEM = "Freq.";
const char s7[] PROGMEM = "Frames";
const char s8[] PROGMEM = "Miss";

const char* const PWMScreenRowNames[ROW_COUNT] PROGMEM = { s1, s2, s3, s4, s5, s6, s7, s8 };

const uint8_t Columns[ROW_COUNT] = {
    1, 3, 2, 2, 2, 2, 1, 1 };

PWMScreen::PWMScreen(PPM& ppm) : ppmH(ppm)
{
    update();
}

void PWMScreen::update()
{
    currentData = ppmH.getPWM();
    hasNewData = true;
}

/* TextUI */

void PWMScreen::activate(TextUI* ui)
{
    if (keepActivated) {
        keepActivated = false;
    }
    else {
        ppmH.startPWMScan();
    }
}

void PWMScreen::deactivate(TextUI* ui)
{
    if (!keepActivated) {
        ppmH.stopScan();
    }
}

bool PWMScreen::hasChanged(uint8_t row, uint8_t col)
{
    return hasNewData;
}

void PWMScreen::endRefresh()
{
    hasNewData = false;
}

void PWMScreen::handleEvent(TextUI* ui, Event* e)
{
    if (e->getType() == EVENT_TYPE_KEY) {

        switch (e->getKey()) {
        case KEY_CLEAR: // long Enter
            ui->popScreen();
            e->markProcessed();
            break;

        case KEY_RESET: // long Up
            ppmH.stopScan();
            delay(500);
            ppmH.startPWMScan();
            e->markProcessed();
            break;

        case KEY_ENTER:
            keepActivated = true;
            scopeScreen.enablePWMMode(true);
            ui->pushScreen(&scopeScreen);
            e->markProcessed();
            break;
        }

    }
    else if (e->getType() == EVENT_TYPE_TIMER) {
        update();
    }
}

const char* PWMScreen::getHeader()
{
    return nullptr;
}

const char* PWMScreen::getMenuName()
{
    return TextUI::copyToBuffer((const char*)F("PWM scan"));
}

uint8_t PWMScreen::getRowCount()
{
    return ROW_COUNT;
}

const char* PWMScreen::getRowName(uint8_t row)
{
    return TextUI::copyToBuffer((const char*)pgm_read_ptr(&PWMScreenRowNames[row]));
}

uint8_t PWMScreen::getColCount(uint8_t row)
{
    return Columns[row];
}

void PWMScreen::getValue(uint8_t row, uint8_t col, Cell* cell)
{
    if (row == 0) {
        if (col == 0) {
            cell->setLabel(5, currentData->sync ? F("SYNC") : F("----"), 4);
        }
    }
    else if (row == 1) {
        if (col == 0) {
            cell->setInt32(5, currentData->frameMin_usec, 6, 0, 0);
        }
        else if (col == 1) {
            cell->setInt32(12, currentData->frameMax_usec, 6, 0, 0);
        }
        else {
            cell->setLabel(19, F("us"), 2);
        }
    }
    else if (row == 2) {
        if (col == 0) {
            cell->setInt16(13, currentData->pulseL_usec[currentData->lastUsed], 5, 0, 0);
        }
        else {
            cell->setLabel(19, F("us"), 2);
        }
    }
    else if (row == 3) {
        if (col == 0) {
            cell->setInt16(13, currentData->pulseH_usec[currentData->lastUsed], 5, 0, 0);
        }
        else {
            cell->setLabel(19, F("us"), 2);
        }
    }
    else if (row == 4) {
        if (col == 0) {
            fixfloat2_t duty;
            long H = currentData->pulseH_usec[currentData->lastUsed];
            long L = currentData->pulseL_usec[currentData->lastUsed];
            if( H == 0 ) {
                duty = 0;
            } else if( L == 0 ) {
                duty = 10000;
            } else {
                duty = H * 10000 / (H+L);
            }
            cell->setFloat2(12, duty, 6, 0, 0);
        }
        else {
            cell->setLabel(19, F("%"), 1);
        }
    }
    else if (row == 5) {
        if (col == 0) {
            fixfloat2_t freq;
            long H = currentData->pulseH_usec[currentData->lastUsed];
            long L = currentData->pulseL_usec[currentData->lastUsed];
            if( H == 0 || L == 0) {
                freq = 0;
            } else {
                freq = 100000000L / (H+L); // 2 digits right of dot
            }
            cell->setFloat2(9, freq, 9, 0, 0);
        }
        else {
            cell->setLabel(19, F("Hz"), 2);
        }
    }
    else if (row == 6) {
        if (col == 0) {
            cell->setInt32(11, currentData->frames, 10, 0, 0);
        }
    }
    else if (row == 7) {
        if (col == 0) {
            cell->setInt32(11, currentData->miss, 10, 0, 0);
        }
    }

}

