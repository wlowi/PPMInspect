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

#include "ConfigScreen.h"
#include "EEPROM.h"

#ifdef ENABLE_MEMDEBUG
  #define ROW_COUNT 9
extern size_t memdebug[4];
#else
  #define ROW_COUNT 8
#endif

config_t settings;

const char *ConfigRowNames[ROW_COUNT] = {
    "Pulse min"
   ,"Pulse max"
   ,"Servo min"
   ,"Servo max"
   ,"Sync  min"
   ,"Vppm +/-"
   ,"Vcc  +/-"
   ,"Low Batt"
#ifdef ENABLE_MEMDEBUG
   ,"Memfree"
#endif
};

ConfigScreen::ConfigScreen(PPM &ppm) : ppmH(ppm)
{
    setDefaults();
}

void ConfigScreen::setDefaults() const
{
    memset(&settings, 0, sizeof(config_t));

    settings.pulseValidMin_usec = PULSEVALIDMIN_usec;
    settings.pulseValidMax_usec = PULSEVALIDMAX_USEC;

    settings.servoValidMin_usec = SERVOVALIDMIN_USEC;
    settings.servoValidMax_usec = SERVOVALIDMAX_USEC;

    settings.syncValidMin_usec = SYNCVALIDMIN_usec;

    settings.vppmAdjust = 0;
    settings.vccAdjust = 0;

    settings.lowBattWarn = 74; // 7.4 Volt
}

void ConfigScreen::load()
{
    EEPROM.get(0, settings);
    if (settings.checksum != computeChecksum(&settings))
    {
        setDefaults();
        save();
    }
}

void ConfigScreen::save()
{
    settings.checksum = computeChecksum(&settings);
    EEPROM.put(0, settings);
}

/*
 * Recompute and return block checksum.
 * NOTE: The blocks checksum is NOT updated.
 */
checksum_t ConfigScreen::computeChecksum(config_t *cfg) const
{
    checksum_t checksum = 0;

    uint8_t *p = (uint8_t *)cfg;

    for (uint16_t i = 0; i < (sizeof(config_t) - sizeof(checksum_t)); i++)
    {
        checksum = rotate(checksum);
        checksum ^= (i ^ *(p + i));
    }

    return checksum;
}

/*
 * Left rotate unsigned int value.
 */
checksum_t ConfigScreen::rotate(checksum_t v) const
{
    return ((v >> 15) & 1) | (v << 1);
}

/* TextUI Screen */

const char *ConfigScreen::getHeader()
{
    return "Settings";
}

const char *ConfigScreen::getMenuName()
{
    return getHeader();
}

void ConfigScreen::activate(TextUI *ui)
{

}

void ConfigScreen::deactivate(TextUI *ui)
{
    save();
}

uint8_t ConfigScreen::getRowCount()
{
    return ROW_COUNT;
}

const char *ConfigScreen::getRowName(uint8_t row)
{
    return ConfigRowNames[row];
}

uint8_t ConfigScreen::getColCount(uint8_t row)
{
    return (row == 5 || row == 6) ? 3 : 2;
}

bool ConfigScreen::hasChanged(uint8_t row, uint8_t col) {

  if( (row == 5) && (col == 1)) {    
    fixfloat1_t voltage = ppmH.readADC();
    if( lastVppm != voltage) {
      lastVppm = voltage;
      return true;
    }
  }
  
  if( (row == 6) && (col == 1)) {    
    fixfloat1_t voltage = ppmH.readVCC();
    if( lastVcc != voltage) {
      lastVcc = voltage;
      return true;
    }
  }
  
  return false;
}

void ConfigScreen::getValue(uint8_t row, uint8_t col, Cell *cell)
{
    if (col == 0)
    {
        if (row == 0)
        {
            cell->setInt16(12, settings.pulseValidMin_usec, 0, 0, 10000);
        }
        else if (row == 1)
        {
            cell->setInt16(12, settings.pulseValidMax_usec, 0, 0, 10000);
        }
        else if (row == 2)
        {
            cell->setInt16(12, settings.servoValidMin_usec, 0, 0, 10000);
        }
        else if (row == 3)
        {
            cell->setInt16(12, settings.servoValidMax_usec, 0, 0, 10000);
        }
        else if (row == 4)
        {
            cell->setInt16(12, settings.syncValidMin_usec, 0, 0, 10000);
        }
        else if (row == 5) // Vppm adjust
        {
            cell->setInt8(10, settings.vppmAdjust, 3, -99, 99);
        }
        else if (row == 6) // Vcc adjust
        {
            cell->setInt8(10, settings.vccAdjust, 3, -99, 99);
        }
        else if (row == 7)
        {
            cell->setFloat1(15, settings.lowBattWarn, 4, 30, 150);
        }

#ifdef ENABLE_MEMDEBUG
        else if (row == 8) // Mem
        {
            cell->setInt16(9, gapSize, 4, 0, 0);
        }
#endif
    }
    else if (col == 1)
    {
        if (row == 5) // Vppm adjust
        {
            cell->setFloat1(15, lastVppm, 4, 0, 0);
        }
        else if (row == 6) // Vcc adjust
        {
            cell->setFloat1(15, lastVcc, 4, 0, 0);
        }
        else if (row == 7)
        {
            cell->setLabel(20, F("V"), 1);
        }

#ifdef ENABLE_MEMDEBUG
        else if (row == 8) // Mem
        {
            cell->setInt16(16, gapFree, 5, 0, 0);
        }
#endif
        else
        {
            cell->setLabel(19, F("us"), 2);
        }
    }
    else if (col == 2)
    {
        cell->setLabel(20, F("V"), 1);
    }
}

void ConfigScreen::setValue(uint8_t row, uint8_t col, Cell *cell)
{
    if (row == 0)
    {
        settings.pulseValidMin_usec = cell->getInt16();
    }
    else if (row == 1)
    {
        settings.pulseValidMax_usec = cell->getInt16();
    }
    else if (row == 2)
    {
        settings.servoValidMin_usec = cell->getInt16();
    }
    else if (row == 3)
    {
        settings.servoValidMax_usec = cell->getInt16();
    }
    else if (row == 4)
    {
        settings.syncValidMin_usec = cell->getInt16();
    }
    else if (row == 5)
    {
        settings.vppmAdjust = cell->getInt8();
    }
    else if (row == 6)
    {
        settings.vccAdjust = cell->getInt8();
    }
    else if (row == 7)
    {
        settings.lowBattWarn = cell->getFloat1();
    }
}
