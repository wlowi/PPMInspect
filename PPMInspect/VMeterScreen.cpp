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

#include "VMeterScreen.h"

VMeterScreen::VMeterScreen(PPM &ppm) : ppmH(ppm)
{
    reset();
    update();
}

void VMeterScreen::reset()
{
  voltage = voltageMax = 0;
  voltageMin = 999;
  hasNewData = true;
}

void VMeterScreen::update()
{
    fixfloat1_t v;
    
    v = ppmH.readADC();
    
    if( v != voltage) {
      voltage = v;
      hasNewData = true;

      if( v < voltageMin) {
        voltageMin = v;
      }
      if( voltage > voltageMax) {
        voltageMax = v;
      }
    }
}

/* TextUI */

void VMeterScreen::activate(TextUI *ui)
{
    /* Set large font size and center output*/
    ui->getDisplay()->setFontSize(TEXTUI_FONT_LARGE);
    xpos = ui->getDisplay()->getColumns() / 2 - 2;
}

void VMeterScreen::deactivate(TextUI *ui)
{
    ui->getDisplay()->setFontSize(TEXTUI_FONT_SMALL);
}

void VMeterScreen::handleEvent(TextUI *ui, Event *e)
{
    if (e->getType() == EVENT_TYPE_KEY)
    {
        switch (e->getKey())
        {
        case KEY_CLEAR:
        case KEY_ENTER:
            ui->popScreen();
            e->markProcessed();
            break;

        case KEY_RESET:
            reset();
            e->markProcessed();
            break;

        case KEY_DOWN:
            enableMinMax = !enableMinMax;
            hasNewData = true;
            e->markProcessed();
            break;

        }
    }
    if (e->getType() == EVENT_TYPE_TIMER || (enableMinMax && e->getType() == EVENT_TYPE_NONE))
    {
        update();
    }
}

const char *VMeterScreen::getHeader()
{
    return "Voltmeter";
}

const char *VMeterScreen::getMenuName()
{
    return getHeader();
}

uint8_t VMeterScreen::getRowCount()
{
    return 3;
}

const char *VMeterScreen::getRowName(uint8_t row)
{
    return "";
}

uint8_t VMeterScreen::getColCount(uint8_t row)
{
    return (row == 0) ? 0 : 2;
}

bool VMeterScreen::hasChanged(uint8_t row, uint8_t col)
{
    return hasNewData;
}

void VMeterScreen::endRefresh()
{
    hasNewData = false;
}

void VMeterScreen::getValue(uint8_t row, uint8_t col, Cell *cell)
{
    if (row == 1)
    {
        if (col == 0)
        {
            cell->setFloat1(xpos, voltage, 4, 0, 0);
        }
        else if (col == 1)
        {
            cell->setLabel(xpos + 5, F("V"), 1);
        }
    }
    else if (row == 2)
    {
        if (col == 0)
        {
            if( enableMinMax) {
                cell->setFloat1( 0, voltageMin, 4, 0, 0);
            } else {
                cell->setLabel( 0, F(" "), 4);
            }
        }
        else if (col == 1)
        {
            if( enableMinMax) {
                cell->setFloat1( 6, voltageMax, 4, 0, 0);
            } else {
                cell->setLabel( 6, F(" "), 4);
            }
        }
    }

}

void VMeterScreen::setValue(uint8_t row, uint8_t col, Cell *cell)
{
    /* noop */
}
