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

#ifndef _PWMScreen_h_
#define _PWMScreen_h_

#include "TextUI.h"
#include "PPM.h"


class PWMScreen : public TextUIScreen
{
private:
    PPM &ppmH;
    pwm_t *currentData = nullptr;
    bool hasNewData = true;
    bool keepActivated = false;

public:
    explicit PWMScreen(PPM &ppm);

    void update();

    /* TextUI */
    void activate(TextUI *ui);
    void deactivate(TextUI *ui);

    void handleEvent(TextUI *ui, Event *e);
    
    bool hasChanged(uint8_t row, uint8_t col);
    void endRefresh();

    const char *getHeader();
    const char *getMenuName();

    uint8_t getRowCount();
    const char *getRowName(uint8_t row);

    uint8_t getColCount(uint8_t row);

    void getValue(uint8_t row, uint8_t col, Cell *cell);
};

#endif