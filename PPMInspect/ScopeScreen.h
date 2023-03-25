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

#ifndef _ScopeScreen_h_
#define _ScopeScreen_h_

#include "TextUI.h"
#include "PPM.h"


#define ARRAY_SZ     ((byte)128)

class ScopeScreen : public TextUIScreen
{
private:
    PPM &ppmH;

    uint8_t dataArray[ARRAY_SZ];

    uint8_t resolution = 3; // 100 usec
    uint8_t triggerMode = 0; // free running
    fixfloat1_t triggerLevel = 20; // 2.0V
    uint16_t triggerDelay = 0;
    uint8_t oversampling = 0; // disable oversampling
    boolean grid = true;
    boolean marker = false;
    
public:
    explicit ScopeScreen(PPM &ppm);

    void update(TextUI *ui);
    uint16_t resToUSec( uint8_t res);

    /* TextUI */
    void handleEvent(TextUI *ui, Event *e);

    const char *getHeader();
    const char *getMenuName();

    bool goBackItem() { return false; }

    uint8_t getRowCount();
    const char *getRowName(uint8_t row);

    bool isRowEditable(uint8_t row) { return true; }
    bool isColEditable(uint8_t row, uint8_t col) { return true; }

    uint8_t getColCount(uint8_t row);

    void getValue(uint8_t row, uint8_t col, Cell *cell);
    void setValue(uint8_t row, uint8_t col, Cell *cell);
};

#endif
