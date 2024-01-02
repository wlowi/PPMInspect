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

#include "ScopeScreen.h"

/* Config */
extern config_t settings;

/*
 * |012345678901234567891|
 * |100uO2V T+2.0V D1000u|
 */

#define ROW_COUNT 1

#define TRIGGER_MODES 3

const char *triggerModes[TRIGGER_MODES] = {
    "f", "+", "-"
};

#define RESOLUTION_STEPS 7

/* Resolution per grid division */
const char *resolutionSteps[RESOLUTION_STEPS] = {
    "100u","200u","500u",
    "  1m","  2m","  5m",
    " 10m"
};

#define OVERSAMPLING_STEPS 2

const char *oversamplingSteps[OVERSAMPLING_STEPS] = {
    " ",
    "O"
};

#define RANGE_STEPS 2

const char *rangeSteps[RANGE_STEPS] = {
    "2V",
    "1V"
};

ScopeScreen::ScopeScreen(PPM &ppm) : ppmH(ppm)
{

}

void ScopeScreen::update(TextUI *ui)
{    
    TextUILcd *lcd = ui->getDisplay();
    uint8_t dataArray[ARRAY_SZ];

    long scaled;
    long divisor;
    
    /* The array gotten from fetchArray() is not adjusted. It is raw data from ADC.
     * 15V input is approximately 255.
     */
    if( ppmH.fetchArray( dataArray, ARRAY_SZ, resToUSec(resolution), oversampling, triggerMode, triggerLevel, triggerDelay * 100)) {
            
        /* We need to map 15.7V == 255 to 2V == 8 because the grid size in Y direction is 8 pixel
         * and we want 2V per Y grid division.
         * 
         * 2V == 8 -> 15.7V == 62.8 -> 255/62.8 = 4.06
         */
        if( range == 0 ) { /* 2V / div */
          divisor = 406;
        } else if( range == 1) { /* 1V / div */
          divisor = 203;
        }
         
        for( uint8_t x = 0; x < ARRAY_SZ; x++) {
            scaled = (long)dataArray[x] * 1000 / (1000 + 4*settings.vppmAdjust);           
            scaled = scaled * 100 / divisor;
            dataArray[x] = (scaled > 55) ? 55 : scaled;
        }

        lcd->drawGrid( dataArray, ARRAY_SZ, 0, 8, 127, 63, grid ? 10 : 0, marker);
        marker = !marker;
    }
}

/* Resolution per pixel */
uint16_t ScopeScreen::resToUSec( uint8_t res) {

  switch (res) {
    case 0: return 10;
    case 1: return 20;
    case 2: return 50;
    case 3: return 100;
    case 4: return 200;
    case 5: return 500;
    case 6: return 1000;
  }

  return 100;
}

/* TextUI */

const char *ScopeScreen::getHeader()
{
    return nullptr;
}

const char *ScopeScreen::getMenuName()
{
    return TextUI::copyToBuffer( (const char*)F("MicroScope"));
}

uint8_t ScopeScreen::getRowCount()
{
    return ROW_COUNT;
}

const char *ScopeScreen::getRowName(uint8_t row)
{
    return "";
}

void ScopeScreen::handleEvent(TextUI *ui, Event *e)
{
  if( ! ui->inEditMode()) {
  
    if (e->getType() == EVENT_TYPE_KEY)
    {

        switch (e->getKey())
        {
        case KEY_CLEAR:
        case KEY_UP:
            ui->popScreen();
            e->markProcessed();
            break;
            
        case KEY_DOWN:
            grid = !grid;
            e->markProcessed();
            break;
        }
    }
    else if (e->getType() == EVENT_TYPE_TICK)
    {
        update( ui);
    } 
    
  }
}

uint8_t ScopeScreen::getColCount(uint8_t row)
{
    return 9;
}

void ScopeScreen::getValue(uint8_t row, uint8_t col, Cell *cell)
{
        if (col == 0) {
            cell->setList(0, resolutionSteps, RESOLUTION_STEPS, resolution);
        } else if( col == 1) {
            cell->setList(4, oversamplingSteps, OVERSAMPLING_STEPS, oversampling);
        } else if( col == 2) {
            cell->setList(5, rangeSteps, RANGE_STEPS, range);
        } else if( col == 3) {
            cell->setLabel(8, F("T"), 1);
        } else if (col == 4) {
            cell->setList(9, triggerModes, TRIGGER_MODES, triggerMode);
        } else if (col == 5) {
            cell->setFloat1(10, triggerLevel, 3, 1, 99);
        } else if (col == 6) {
            cell->setLabel(13, F("V D"), 3);
        } else if (col == 7) {
            cell->setInt16(16, triggerDelay, 2, 0, 99);
        } else if (col == 8) {
            cell->setLabel(18, F("00u"), 3);
        }

}

void ScopeScreen::setValue(uint8_t row, uint8_t col, Cell *cell)
{
    if( col == 0) {
        resolution = cell->getList();
    } else if( col == 1) {
        oversampling = cell->getList();
    } else if( col == 2) {
        range = cell->getList();
    } else if( col == 4) {
        triggerMode = cell->getList();
    } else if( col == 5) {
        triggerLevel = cell->getFloat1();
    }  else if( col == 7) {
        triggerDelay = cell->getInt16();
    }
}
