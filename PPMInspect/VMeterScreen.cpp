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
    update();
}

void VMeterScreen::update()
{
    voltage = ppmH.readADC();
    hasNewData = true;
}

/* TextUI */

void VMeterScreen::activate(TextUI *ui)
{
    /* Set large font size and center output*/
    ui->getDisplay()->setFontSize(TEXTUI_FONT_LARGE);
    xpos = ui->getDisplay()->getColumns() / 2 - 2;

    ppmH.initADC();
}

void VMeterScreen::deactivate(TextUI *ui)
{
    ui->getDisplay()->setFontSize(TEXTUI_FONT_SMALL);
}

void VMeterScreen::handleEvent(TextUI *ui, Event *e)
{
    if (e->getType() == EVENT_TYPE_TIMER)
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
    return 2;
}

const char *VMeterScreen::getRowName(uint8_t row)
{
    return "";
}

bool VMeterScreen::isRowExecutable(uint8_t row)
{
    return true;
}

void VMeterScreen::rowExecute(TextUI *ui, uint8_t row)
{
    ui->popScreen();
}

bool VMeterScreen::isRowEditable(uint8_t row)
{
    return false;
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
}

void VMeterScreen::setValue(uint8_t row, uint8_t col, Cell *cell)
{
    /* noop */
}
