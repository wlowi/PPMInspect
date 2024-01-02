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

#include "DataScreen.h"
#include "ChannelScreen.h"

extern ChannelScreen channelScreen;

#define ROW_COUNT 9

const char s1[] PROGMEM = "PPM";
const char s2[] PROGMEM = "Frame";
const char s3[] PROGMEM = "Pulse";
const char s4[] PROGMEM = "Pulse";
const char s5[] PROGMEM = "Level";
const char s6[] PROGMEM = "Frames";
const char s7[] PROGMEM = "E: Long frame";
const char s8[] PROGMEM = "E: Ch. count";
const char s9[] PROGMEM = "E: Pulse time";

const char* const DataScreenRowNames[ROW_COUNT] PROGMEM = { s1, s2, s3, s4, s5, s6, s7, s8, s9 };

const uint8_t Columns[ROW_COUNT] = {
    3, 3, 3, 1, 4, 1, 1, 1, 1 };

DataScreen::DataScreen(PPM& ppm) : ppmH(ppm)
{
    update();
}

void DataScreen::update()
{
    currentData = ppmH.getPPM();
    hasNewData = true;
}

/* TextUI */

void DataScreen::activate(TextUI* ui)
{
    if (keepActivated) {
        keepActivated = false;
    }
    else {
        ppmH.startPPMScan();
    }
}

void DataScreen::deactivate(TextUI* ui)
{
    if (!keepActivated) {
        ppmH.stopScan();
    }
}

const char* DataScreen::getHeader()
{
    return nullptr;
}

const char* DataScreen::getMenuName()
{
    return TextUI::copyToBuffer((const char*)F("PPM scan"));
}

uint8_t DataScreen::getRowCount()
{
    return ROW_COUNT;
}

const char* DataScreen::getRowName(uint8_t row)
{
    return TextUI::copyToBuffer((const char*)pgm_read_ptr(&DataScreenRowNames[row]));
}

void DataScreen::handleEvent(TextUI* ui, Event* e)
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
            ppmH.startPPMScan();
            e->markProcessed();
            break;

        case KEY_ENTER:
            keepActivated = true;
            ui->pushScreen(&channelScreen);
            e->markProcessed();
            break;
        }
    }
    else if (e->getType() == EVENT_TYPE_TIMER) {
        update();
    }
}

uint8_t DataScreen::getColCount(uint8_t row)
{
    return Columns[row];
}

bool DataScreen::hasChanged(uint8_t row, uint8_t col)
{
    return hasNewData;
}

void DataScreen::endRefresh()
{
    hasNewData = false;
}

void DataScreen::getValue(uint8_t row, uint8_t col, Cell* cell)
{
    if (row == 0) {
        if (col == 0) {
            cell->setLabel(5, currentData->sync ? F("SYNC") : F("----"), 4);
        }
        else if (col == 1) {
            cell->setInt8(9, currentData->channels, 3, 0, 0);
        }
        else {
            cell->setLabel(13, F("Channels"), 8);
        }
    }
    else if (row == 1) {
        if (col == 0) {
            cell->setInt16(7, currentData->frameMin_usec, 5, 0, 0);
        }
        else if (col == 1) {
            cell->setInt16(13, currentData->frameMax_usec, 5, 0, 0);
        }
        else {
            cell->setLabel(19, F("us"), 2);
        }
    }
    else if (row == 2) {
        if (col == 0) {
            cell->setInt16(7, currentData->pulseMin_usec, 5, 0, 0);
        }
        else if (col == 1) {
            cell->setInt16(13, currentData->pulseMax_usec, 5, 0, 0);
        }
        else {
            cell->setLabel(19, F("us"), 2);
        }
    }
    else if (row == 3) {
        if (col == 0) {
            cell->setLabel(8, currentData->pulseLevel ? F("HIGH") : F(" LOW"), 4);
        }
    }
    else if (row == 4) {
        if (col == 0) {
            cell->setFloat1(7, currentData->vLevel_low, 4, 0, 0);
        }
        else if (col == 1) {
            cell->setLabel(11, F("V"), 1);
        }
        else if (col == 2) {
            cell->setFloat1(16, currentData->vLevel_high, 4, 0, 0);
        }
        else {
            cell->setLabel(20, F("V"), 1);
        }
    }
    else if (row == 5) {
        if (col == 0) {
            cell->setInt32(11, currentData->frames, 10, 0, 0);
        }
    }
    else if (row == 6) {
        if (col == 0) {
            cell->setInt16(16, currentData->badFrames, 5, 0, 0);
        }
    }
    else if (row == 7) {
        if (col == 0) {
            cell->setInt16(16, currentData->badCount, 5, 0, 0);
        }
    }
    else if (row == 8) {
        if (col == 0) {
            cell->setInt16(16, currentData->badPulse, 5, 0, 0);
        }
    }
}

void DataScreen::setValue(uint8_t row, uint8_t col, Cell* cell)
{
    /* noop */
}
