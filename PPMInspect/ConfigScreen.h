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

#ifndef _ConfigScreen_h_
#define _ConfigScreen_h_

#include "Config.h"
#include "TextUI.h"
#include "PPM.h"

class ConfigScreen : public TextUIScreen
{
private:
    PPM &ppmH;
    checksum_t computeChecksum(config_t *cfg) const;
    checksum_t rotate(checksum_t v) const;
    fixfloat1_t lastVppm = 0;
    fixfloat1_t lastVcc = 0;
    
public:
    ConfigScreen(PPM &ppm);

    void load();
    void save();

    void setDefaults() const;

    /* TextUIScreen */
    const char *getHeader();
    const char *getMenuName();

    bool goBackItem() { return true; }

    void activate(TextUI *ui);
    void deactivate(TextUI *ui);

    bool isRowEditable(uint8_t row) { return (row < 8); }
    bool isColEditable(uint8_t row, uint8_t col) { return (col == 0); }

    uint8_t getRowCount();
    const char *getRowName(uint8_t row);
    uint8_t getColCount(uint8_t row);

    bool hasChanged(uint8_t row, uint8_t col);
    void getValue(uint8_t row, uint8_t col, Cell *cell);
    void setValue(uint8_t row, uint8_t col, Cell *cell);
};

#endif
