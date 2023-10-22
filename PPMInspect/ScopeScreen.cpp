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

/*
 * |012345678901234567891|
 * |1000uOT+ 10.0V 1000u |
 */

#define ROW_COUNT 1

#define TRIGGER_MODES 3

const char *triggerModes[TRIGGER_MODES] = {
    "F", "+", "-"
};

#define RESOLUTION_STEPS 7

const char *resolutionSteps[RESOLUTION_STEPS] = {
    "  10","  20","  50",
    " 100"," 200"," 500",
    "1000"
};

#define OVERSAMPLING_STEPS 2

const char *oversamplingSteps[OVERSAMPLING_STEPS] = {
    " ",
    "O"
};


ScopeScreen::ScopeScreen(PPM &ppm) : ppmH(ppm)
{

}

void ScopeScreen::update(TextUI *ui)
{    
    TextUILcd *lcd = ui->getDisplay();

    if( ppmH.fetchArray( dataArray, ARRAY_SZ, resToUSec(resolution), oversampling, triggerMode, triggerLevel, triggerDelay * 100)) {

        for( uint8_t x = 0; x < ARRAY_SZ; x++) {
             /* Divide by 5 to map range [0,255] to grid of height [10,63] (approximately) */
            dataArray[x] /= 5;
        }

        lcd->drawGrid( dataArray, ARRAY_SZ, 0, 10, 127, 63, grid ? 10 : 0, marker);
        marker = !marker;
    }
}

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
    return "MicroScope";
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
            cell->setLabel(4, "u", 1);
        } else if( col == 2) {
            cell->setList(5, oversamplingSteps, OVERSAMPLING_STEPS, oversampling);
        } else if( col == 3) {
            cell->setLabel(6, "T", 1);
        } else if (col == 4) {
            cell->setList(7, triggerModes, TRIGGER_MODES, triggerMode);
        } else if (col == 5) {
            cell->setFloat1(9, triggerLevel, 4, 1, 200);
        } else if (col == 6) {
            cell->setLabel(13, "V D", 3);
        } else if (col == 7) {
            cell->setInt16(16, triggerDelay, 2, 0, 99);
        } else if (col == 8) {
            cell->setLabel(18, "00u", 3);
        }

}

void ScopeScreen::setValue(uint8_t row, uint8_t col, Cell *cell)
{
    if( col == 0) {
        resolution = cell->getList();
    } else if( col == 2) {
        oversampling = cell->getList();
    } else if( col == 4) {
        triggerMode = cell->getList();
    } else if( col == 5) {
        triggerLevel = cell->getFloat1();
    }  else if( col == 7) {
        triggerDelay = cell->getInt16();
    }
}
