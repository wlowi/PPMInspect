/*
  TextUI. A simple text based UI.

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

#include "TextUILcdSSD1306.h"

TextUILcdSSD1306::TextUILcdSSD1306( const DevType *device ) {

#ifdef TEXTUI_LCD_USE_AVRI2C

  delay(100);
  lcd.begin( device, DISPLAY_I2C_ADDRESS);

#else

  Wire.begin();
  Wire.setClock(400000L);

  Wire.beginTransmission( DISPLAY_I2C_ADDRESS);
  
  if (Wire.endTransmission() == 0) {
    lcd.begin( device, DISPLAY_I2C_ADDRESS);
  }

#endif
  delay(100);
    
  lcd.setFont(Adafruit5x7);
  setFontSize( TEXTUI_FONT_SMALL);           // Normal 1:1 pixel scale

  clear();
  setCursor(0, 0);
}

void TextUILcdSSD1306::clear() {

  lcd.clear();
}

void TextUILcdSSD1306::clear( uint8_t x0, uint8_t r0, uint8_t x1, uint8_t r1) {

  lcd.clear( x0, x1, r0, r1);
}

void TextUILcdSSD1306::clearEOL() {

  lcd.clearToEOL();
}

bool TextUILcdSSD1306::colorSupport() {

  return false;
}
    
void TextUILcdSSD1306::setBg( uint8_t r, uint8_t g, uint8_t b) {

  /* noop */
}

void TextUILcdSSD1306::setFg( uint8_t r, uint8_t g, uint8_t b) {

  /* noop */
}

void TextUILcdSSD1306::normalColors() {

  setInvert( false);
}

void TextUILcdSSD1306::selectedColors() {

  setInvert( true);
}

void TextUILcdSSD1306::editColors() {

  setInvert( true);
}

void TextUILcdSSD1306::setInvert( bool inv) {

  lcd.setInvertMode( inv);
}

void TextUILcdSSD1306::setFontSize( FontSize_t sz) {

  if( sz == TEXTUI_FONT_SMALL) {
    lcd.set1X();
  } else {
    lcd.set2X();
  }
}
    
uint8_t TextUILcdSSD1306::getRows() {

  return lcd.displayHeight() / (8 * lcd.fontRows());
}

uint8_t TextUILcdSSD1306::getColumns() {

  return lcd.displayWidth() / (lcd.fontWidth() + lcd.letterSpacing());
}
    
void TextUILcdSSD1306::setCursor( uint8_t r, uint8_t c) {

  setRow( r);
  setColumn( c);
}

void TextUILcdSSD1306::setRow( uint8_t r) {

  lcd.setRow( r * lcd.fontRows());
}

void TextUILcdSSD1306::setColumn( uint8_t c) {

  lcd.setCol( c * (lcd.fontWidth() + lcd.letterSpacing()));
}

void TextUILcdSSD1306::printChar( char ch) {

  lcd.write( ch);
}

/* Draw a graph optionally with grid.
 * Y grid size is always 8.
 * 
 * dataArray[]  - Data
 * sz           - Size of dataArray
 * si           - Array start index
 * x0           - Start x/y (0,0 is upper left corner)
 * y0
 * x1           - End x/y
 * y1
 * gridX        - Grid size in X direction, 0 disables grid
 * marker       - Draw update marker 
 */
void TextUILcdSSD1306::drawGrid( uint8_t dataArray[], uint8_t sz, uint8_t si, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t gridX, boolean marker)
{
  uint8_t m;
  uint8_t y;
  uint8_t prevY;
  uint8_t x;
  uint8_t ii;

  uint8_t startRow;
  uint8_t endRow;
  uint8_t row;
  
  uint8_t d0Row;
  uint8_t d1Row;
  uint8_t d0y;
  uint8_t d1y;
  
  startRow = y0 / 8;
  endRow = y1 / 8;

  for( row = startRow; row <= endRow; row++) {
    lcd.setCol( x0);
    lcd.setRow( row);

    prevY = y1 - dataArray[si];
    ii = 1;

    for( x = x0; (x <= x1) && (ii < sz); x++) {
      y = y1 - dataArray[(si + ii) % sz];
      ii++;

      /* Compute row and y value of the vertical line to draw.
       * d0 has the lower numeric value.
       */
      if( y < prevY) {
        d0y = y; d1y = prevY;
      } else {
        d0y = prevY; d1y = y;
      }

      d0Row = d0y / 8;
      d1Row = d1y / 8;

      if( row == d0Row && d0Row == d1Row) {
        /* Start (d0y) and end (d1y) are within the same row.
         * Draw a line from d0y to d1y.
         */
        m = 255 - ((1 << (d0y % 8)) -1);
        m &= (1 << ((d1y % 8) +1)) -1;
        
      } else if( row == d0Row) {
        /* Start (d0y) is within row.
         * Draw a line from d0y to end of row.
         */
        m = 255 - ((1 << (d0y % 8)) -1);
        
      } else if( row == d1Row) {
        /* End (d1y) is within row.
         * Draw a line from start to d1y.
         */
        m = (1 << ((d1y % 8) +1)) -1;
        
      } else if (row < d0Row || row > d1Row) {
          /* Outside of d0y - d1y. */
          m = 0;
          
      } else {
        /* Inside of d0y - d1y. */
        m = 0xff;
      }

      if( gridX && ((x % gridX) == 0)) {
        m |= 0b10000000; 
      }
        
      lcd.ssd1306WriteRam( m);
      prevY = y;
    }

    for( ; x <= x1; x++) {
      lcd.ssd1306WriteRam( 0);
    }
  }

  if( marker) {
    lcd.setCol( x1);
    lcd.setRow( startRow);
    lcd.ssd1306WriteRam( 0x0f);
  }
}
