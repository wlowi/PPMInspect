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

#include "ChannelScreen.h"

ChannelScreen::ChannelScreen(PPM &ppm) : ppmH(ppm)
{
    strcpy( channelName, "C  ");
    update();
}

void ChannelScreen::update()
{
    currentData = ppmH.getPPM();
    hasNewData = true;
}

/* TextUI */

const char *ChannelScreen::getHeader()
{
    return nullptr;
}

const char *ChannelScreen::getMenuName()
{
    return "Channels";
}

uint8_t ChannelScreen::getRowCount()
{
    return currentData->channels + 1; 
}

const char *ChannelScreen::getRowName(uint8_t row)
{
    if( row == 0) {
        return "PPM";
    }

    if( row < 10) {
      channelName[1] = '0' + row;
      channelName[2] = ' ';
    } else {
      channelName[1] = '1';
      channelName[2] = '0' + row -10;
    }

    return channelName;
}

void ChannelScreen::handleEvent(TextUI *ui, Event *e)
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
        } 
    }
    else if (e->getType() == EVENT_TYPE_TIMER)
    {
        update();
    } 
}

uint8_t ChannelScreen::getColCount(uint8_t row)
{
    return 3;
}

bool ChannelScreen::hasChanged(uint8_t row, uint8_t col)
{
    return hasNewData;
}

void ChannelScreen::endRefresh()
{
    hasNewData = false;
}

void ChannelScreen::getValue(uint8_t row, uint8_t col, Cell *cell)
{
    if (row == 0) {

        if (col == 0) {
            cell->setLabel(5, currentData->sync ? F("SYNC") : F("----"), 4);
        } else if (col == 1) {
            cell->setInt8(9, currentData->channels, 3, 0, 0);
        } else {
            cell->setLabel(13, F("Channels"), 8);
        }
        
    } else {
      
        if (col == 0) {
            cell->setInt16(4, currentData->channel_usec[row-1], 5, 0, 0);
        } else if (col == 1) {
            cell->setInt16(10, currentData->channelMin_usec[row-1], 5, 0, 0);
        } else {
            cell->setInt16(16, currentData->channelMax_usec[row-1], 5, 0, 0);
        }
    }
}

void ChannelScreen::setValue(uint8_t row, uint8_t col, Cell *cell)
{
    /* noop */
}
