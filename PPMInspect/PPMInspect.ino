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

/*
 *  PPMInspect
 *
 * Arduino Pro Micro port assignment
 * =================================
 *   LCD SCK    3
 *   LCD SDA    2
 *   Button 1  14
 *   Button 2  15
 *   Button 3  16
 *   Analog In  ?
 *   PPM In     ?
 *   Vcc       A0
 *   
 * Arduino Nano port assignment
 * ============================
 *   LCD SCK   A5 (Port PC5 SCL)
 *   LCD SDA   A4 (Port PC4 SDA)
 *   Button 1   5 (Port PD5 PCINT21 PCI2)
 *   Button 2   6 (Port PD6 PCINT22 PCI2)
 *   Button 3   7 (Port PD7 PCINT23 PCI2)
 *   Analog In A3 (Port PC3)
 *   PPM In     8 (Port PB0 ICP1)
 *   Vcc       A0
 *   
 * Timer
 * =====
 *   Timer 0   8 bit       Arduino micros() millis() delay()...
 *   Timer 1  16 bit       PPM Timing (See PPM.cpp)
 */

#include "Config.h"
#include "TextUI.h"

#include "TextUILcdSSD1306.h"
#include "TextUISimpleKbd.h"

#include "PPM.h"

#include "HomeScreen.h"
#include "DataScreen.h"
#include "ScopeScreen.h"
#include "ChannelScreen.h"
#include "VMeterScreen.h"
#include "ConfigScreen.h"


#ifdef ENABLE_MEMDEBUG
size_t memdebug[4];
#endif


TextUI textUI;

extern PPM ppm;
extern config_t settings;

DataScreen dataScreen(ppm);
ScopeScreen scopeScreen(ppm);
ChannelScreen channelScreen(ppm);
VMeterScreen vMeterScreen(ppm);
ConfigScreen configScreen(ppm);

uint8_t buttons[BUTTON_COUNT] = { BUTTON_PORTS };
uint8_t skeys[BUTTON_COUNT] = { BUTTON_SHORT_KEYS };
uint8_t lkeys[BUTTON_COUNT] = { BUTTON_LONG_KEYS };

void setup()
{
    configScreen.load();

    textUI.setDisplay(new TextUILcdSSD1306( &SH1106_128x64 ));
    textUI.setInput(new TextUISimpleKbd(BUTTON_COUNT, buttons, skeys, lkeys));
    textUI.setReversedNav( true);

    textUI.setTimer(500);
    textUI.setHomeScreen(new HomeScreen());

#ifdef ENABLE_MEMDEBUG
    MEMDEBUG_INIT();
#endif
}

void loop()
{
    Event *e = textUI.getEvent();

    if( checkBattery(e)) {
        textUI.handle(e);
    }
    
#ifdef ENABLE_MEMDEBUG
    MEMDEBUG_CHECK();
#endif
}

/*
 * Check Battery.
 * Returns false on battery warning.
 */
bool checkBattery(Event *e)
{
    fixfloat1_t v;
    TextUILcd *lcd;
    static int8_t count = 0; // Counts timer timeouts (500msec)

    if( e->getType() == EVENT_TYPE_TIMER) {

        count++;
        
        if( count == 0) {
            // Switch back to regular display
            textUI.forceRefresh();
        
        } else if( count >= 20) { // 10 Seconds
            
            v = ppm.readVCC();
        
            if( v < settings.lowBattWarn ) {
                // Show battery warning
                lcd = textUI.getDisplay();
                lcd->clear();
                lcd->printStr( F("LOW BAT!"));
                count = -2;
                return false;
          
            } else { // Reset counter
                count = 0;
            }
        
        } else if( count < 0) {
            return false;
        }
    } else {
        if( count < 0) {
          return false;
        }
    }
    
    return true;
}
