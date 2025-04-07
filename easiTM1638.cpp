/*!
 * An Easy TM1638 Arduino Library.
 *  Simple, functional, optimal, and all in a class!
 *
 * The TM1638 is an (up to) 8-Digit 7-Segment (+dps) LED display driver.
 *
 * Written for the Arduino Uno/Nano/Mega.
 * (c) Ian Neill 2025
 * GPL(v3) Licence
 *
 * Built on my TM1637 library, and work by Gavin Lyons and Handson Technology.
 * ... with some developments backported into this library from my MAX7219 library.
 *
 * References:
 *    https://github.com/gavinlyonsrepo/TM1638plus
 *    https://www.handsontec.com/dataspecs/display/TM1638.pdf
 *    https://www.makerguides.com/connecting-arduino-uno-with-tm1638-7-segment-led-driver/
 *    https://jetpackacademy.com/wp-content/uploads/2018/06/TM1638_cheat_sheet_download.pdf
 *
 ***************************************************************
 * LED Segments:         a
 *                     -----
 *                   f|     |b
 *                    |  g  |
 *                     -----
 *                   e|     |c
 *                    |     |
 *                     -----  o dp
 *                       d
 *   Register bits:
 *      bit:  7   6  5  4  3  2  1  0
 *            dp  g  f  e  d  c  b  a
 ***************************************************************
 *
 * ****************************
 * * easiTM1638 C++ Code File *
 * ****************************
 */

#include "easiTM1638.h"

// A table of 7-segment character codes (47 in total).
uint8_t TM1638::tmCharTable[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67, // Numbers : 0-9.
                                 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71,                         // Numbers : A, b, C, d, E, F.
                                 0x58, 0x6f, 0x74, 0x76, 0x10, 0x30, 0x1e, 0x38,             // Chars1  : c, g, h, H, i, I, J, L.
                                 0x54, 0x37, 0x73, 0x50, 0x78, 0x1c, 0x3e, 0x6e,             // Chars2  : n, N, P, r, t, u, U, y.
                                 0x00,                                                       // Blank   : Space = index 32 (0x20). 
                                 0x01, 0x40, 0x08, 0x63, 0x5c, 0x46, 0x70,                   // Specials: uDash, mDash, lDash, uBox, lBox, lBorder, rBorder.
                                 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};                  // Segments: SegA, SegB, SegC, SegD, SegE, SegF, SegG.

// A table to describe the physical to logical digit numbering.
// This map assumes that the digits are logically addressed in the same order as they are physically built.
uint8_t TM1638::tmDigitMapDefault[] = {0, 1, 2, 3, 4, 5, 6, 7};


/**************************/
/* Public Class Functions */
/**************************/

// Class constructor.
TM1638::TM1638(uint8_t stbPin, uint8_t clkPin, uint8_t dataPin) {
  _clkPin  = clkPin;                                      // Record the TM1638 clock pin.
  _dataPin = dataPin;                                     // Record the TM1638 data pin.
  _stbPin  = stbPin;                                      // Record the TM1638 strobe pin.
  // Calculate the size of the character code table.
  charTableSize = (sizeof(tmCharTable) / sizeof(*tmCharTable));
}

// Set up the display and initialise it with defaults values - with the default digit map.
void TM1638::begin(uint8_t numButtons, uint8_t numLEDs, uint8_t numDigits, uint8_t brightness) {
  this->begin(tmDigitMapDefault, numButtons, numLEDs, numDigits, brightness);
}

// Set up the display and initialise it with defaults values - with a supplied digit map.
void TM1638::begin(uint8_t* tmDigitMap, uint8_t numButtons, uint8_t numLEDs, uint8_t numDigits, uint8_t brightness) {
  _tmDigitMap = tmDigitMap;
  if(numLEDs > 0 && numLEDs <= MAX_LEDS38) {              // The TM1638 module supports up to 8 LEDs.
    _numLEDs = numLEDs;
  }
  else {
    _numLEDs = 0;                                         // We have no TM1638 module LEDs.
  }
  if(numDigits > 0 && numDigits <= MAX_DIGITS38) {        // The TM1638 module supports up to 8 digits.
    _numDigits = numDigits;
  }
  else {
    _numDigits = 1;                                       // We have only one TM1638 module digit.
  }
  if(numButtons > 0 && numButtons <= MAX_BUTTONS38) {     // The TM1638 module supports up to 8 buttons.
    _numButtons = numButtons;
  }
  else {
    _numButtons = 0;                                      // We have no TM1638 module buttons.
  }
  pinMode(_clkPin, OUTPUT);                               // Set up the clock pin for output.
  pinMode(_dataPin, OUTPUT);                              // Set up the data pin for output.
  pinMode(_stbPin, OUTPUT);                               // Set up the strobe pin for output.
  this->displayClear();                                   // Clear the LEDS and display, all segments and decimal points.
  this->displayBrightness(brightness);                    // Set the display to the chosen (or default) brightness.
}

// Turn the TM1638 display OFF.
void TM1638::displayOff(void) {
  cmdDispCtrl = DISP_OFF38;                               // 0x80 = display OFF.
  this->writeCommand(cmdDispCtrl);                        // Turn the display OFF.
}

// Clear all the LEDs and digits (+dps) in the display.
void TM1638::displayClear(void) {
  uint8_t digit;
  for(digit = 0; digit < _numDigits; digit++) {
    this->displayLED1(digit, false);                      // Turn OFF all the TM1638 module LEDs.
    this->displayChar(digit, 0x00, true);                 // Write a zero (all segments OFF) to each digit.
    this->displayDP(digit, false);                        // Turn OFF all the decimal points.
  }
}

// Set the brightness (0x00 - 0x07) and turn the TM1638 display ON.
void TM1638::displayBrightness(uint8_t brightness) {
  _brightness = brightness & INTENSITY_MAX38;             // Record the TM1638 brightness level.
  cmdDispCtrl = DISP_ON38 + _brightness;                  // 88 + 0 to 7 brightness, 88 = display ON.
  this->writeCommand(cmdDispCtrl);                        // Set the brightness and turn the display ON.
}

// Test the display - all the display LEDs and digit segments (+dps).
void TM1638::displayTest(bool dispTest) {
  uint8_t digit;
  this->writeCommand(ADDR_AUTO38);                        // Cmd to set auto incrementing address mode.
  this->start();                                          // Send the start signal to the TM1638.
  this->writeByte(STARTADDR38);                           // Set the address to the first digit.
  if(dispTest) {
    // Turn ON all the LEDs, and all digit segments (+dps).
    for(digit = 0; digit < max(_numDigits, _numLEDs); digit++) {
      this->writeByte(0xff);                              // Direct write to turn all digit segments (+dps) ON.
      this->writeByte(0x01);                              // Direct write to turn the LED ON.
    }
  }
  else {
    // Restore all the LEDs, and all digit segments (+dps) to their previous values.
    for(digit = 0; digit < max(_numDigits, _numLEDs); digit++) {
      this->writeByte( _registers[digit]);                // Restore the digit segments (+dps) to what they were.
      this->writeByte((_allLEDs >> digit) & 0x01);        // Restore the LED to what it was.
    }
  }
  this->stop();                                           // Send the stop signal to the TM1638.
}

// Display a binary integer between 0b00000000 - 0b11111111, starting at digit 0 for the LSB or MSB.
void TM1638::displayBin8(uint8_t number, bool lsbFirst) {
  uint8_t digit;
  if(_numDigits > 7) {                                    // We need at least 8 digits to display an 8-bit binary number, leftmost digit is #0.
    for(digit = 0; digit < 8; digit++) {
      if(lsbFirst) {
        _registers[digit] = (_registers[digit] & DP_CTRL38) | (tmCharTable[(number >> digit) & 0x01] & 0x7f);
      }
      else {
        _registers[digit] = (_registers[7 - digit] & DP_CTRL38) | (tmCharTable[(number >> (7 - digit)) & 0x01] & 0x7f);
      }
      this->writeCommand(ADDR_FIXED38);                   // Cmd to set specific address mode.
      this->writeDigit(digit);                            // Write the digit of the 8-bit number to the display.
    }
  }
}

// Display a character in a specific digit.
void TM1638::displayChar(uint8_t digit, uint8_t number, bool raw) {
  if(digit < _numDigits) {                                // Boundry check the digit number, leftmost digit is #0.
    if(raw) {                                             // If this is a raw segment bit number, ensure there are only 7 bits.
      number &= 0x7f;
      number |= (_registers[digit] & DP_CTRL38);          // Merge the segment number with the dp (bit 7) status.
    }
    else {                                                // If using the character table, ensure the number is within the character table.
      if(number >= charTableSize) {
        number = 0x20;                                    // This is a 0x00 (space) in the character table.
      }
      number = tmCharTable[number];                       // Get the raw number from the character table.
      number |= (_registers[digit] & DP_CTRL38);          // Merge the segment number with the dp (bit 7) status.
    }
    _registers[digit] = number;                           // Record the latest value for this LED digit.
    this->writeCommand(ADDR_FIXED38);                     // Cmd to set specific address mode.
    this->writeDigit(digit);                              // Write the character digit to the display.
  }
}

// Display a decimal integer between 0 - 99, or a hex integer between 0x00 - 0xff, starting at a specific digit.
void TM1638::displayInt8(uint8_t digit, uint8_t number, bool useDec) {
  if(_numDigits > 1 && digit < (_numDigits - 1)) {        // We need at least 2 digits to display an 8-bit number, leftmost digit is #0.
    if(useDec) {
      if(number > 99) {                                   // Clip the number at the maximum for a 2 digit decimal number.
        number = 99;
      }
      _registers[digit]     = (_registers[digit]     & DP_CTRL38) | (tmCharTable[(number / 10) % 10] & 0x7f);
      _registers[digit + 1] = (_registers[digit + 1] & DP_CTRL38) | (tmCharTable[ number       % 10] & 0x7f);
    }
    else {
      _registers[digit]     = (_registers[digit]     & DP_CTRL38) | (tmCharTable[(number / 16) % 16] & 0x7f);
      _registers[digit + 1] = (_registers[digit + 1] & DP_CTRL38) | (tmCharTable[ number       % 16] & 0x7f);
    }
    this->writeCommand(ADDR_FIXED38);                     // Cmd to set specific address mode.
    this->writeDigit(digit);                              // Write the first digit of the 8-bit number.
    this->writeDigit(digit + 1);                          // Write the second digit of the 8-bit number.
  }
}

// Display a decimal integer between 0 - 999, or a hex integer between 0x000 - 0xfff, starting at a specific digit.
void TM1638::displayInt12(uint8_t digit, uint16_t number, bool useDec) {
  if(_numDigits > 2 && digit < (_numDigits - 2)) {        // We need at least 3 digits to display an 12-bit number, leftmost digit is #0.
    if(useDec) {
      if(number > 999) {                                  // Clip the number at the maximum for a 3 digit decimal number.
        number = 999;
      }
      _registers[digit]     = (_registers[digit]     & DP_CTRL38) | (tmCharTable[(number / 100) % 10] & 0x7f);
      _registers[digit + 1] = (_registers[digit + 1] & DP_CTRL38) | (tmCharTable[(number /  10) % 10] & 0x7f);
      _registers[digit + 2] = (_registers[digit + 2] & DP_CTRL38) | (tmCharTable[ number        % 10] & 0x7f);
    }
    else {
      if(number > 0xfff) {                                // Clip the number at the maximum for a 3 digit hexadecimal number.
        number = 0xfff;
      }
      _registers[digit]     = (_registers[digit]     & DP_CTRL38) | (tmCharTable[(number / 256) % 16] & 0x7f);
      _registers[digit + 1] = (_registers[digit + 1] & DP_CTRL38) | (tmCharTable[(number /  16) % 16] & 0x7f);
      _registers[digit + 2] = (_registers[digit + 2] & DP_CTRL38) | (tmCharTable[ number        % 16] & 0x7f);
    }
    this->writeCommand(ADDR_FIXED38);                     // Cmd to set specific address mode.
    this->writeDigit(digit);                              // Write the first digit of the 12-bit number.
    this->writeDigit(digit + 1);                          // Write the second digit of the 12-bit number.
    this->writeDigit(digit + 2);                          // Write the third digit of the 12-bit number.
  }
}

// Display a decimal integer between 0 - 9999, or a hex integer between 0x0000 - 0xffff, starting at a specific digit.
void TM1638::displayInt16(uint8_t digit, uint16_t number, bool useDec) {
  if(_numDigits > 3 && digit < (_numDigits - 3)) {        // We need at least 4 digits to display an 16-bit number, leftmost digit is #0.
    if(useDec) {
      if(number > 9999) {                                 // Clip the number at the maximum for a 4 digit decimal number.
        number = 9999;
      }
      _registers[digit]     = (_registers[digit]     & DP_CTRL38) | (tmCharTable[(number / 1000) % 10] & 0x7f);
      _registers[digit + 1] = (_registers[digit + 1] & DP_CTRL38) | (tmCharTable[(number /  100) % 10] & 0x7f);
      _registers[digit + 2] = (_registers[digit + 2] & DP_CTRL38) | (tmCharTable[(number /   10) % 10] & 0x7f);
      _registers[digit + 3] = (_registers[digit + 3] & DP_CTRL38) | (tmCharTable[ number         % 10] & 0x7f);
    }
    else {
      _registers[digit]     = (_registers[digit]     & DP_CTRL38) | (tmCharTable[(number / 4096) % 16] & 0x7f);
      _registers[digit + 1] = (_registers[digit + 1] & DP_CTRL38) | (tmCharTable[(number /  256) % 16] & 0x7f);
      _registers[digit + 2] = (_registers[digit + 2] & DP_CTRL38) | (tmCharTable[(number /   16) % 16] & 0x7f);
      _registers[digit + 3] = (_registers[digit + 3] & DP_CTRL38) | (tmCharTable[ number         % 16] & 0x7f);
    }
    this->writeCommand(ADDR_FIXED38);                     // Cmd to set specific address mode.
    this->writeDigit(digit);                              // Write the first digit of the 16-bit number.
    this->writeDigit(digit + 1);                          // Write the second digit of the 16-bit number.
    this->writeDigit(digit + 2);                          // Write the third digit of the 16-bit number.
    this->writeDigit(digit + 3);                          // Write the fourth digit of the 16-bit number.
  }
}

// Display a binary integer between 0b00000000 - 0b11111111 on the LEDs, starting at LED 0 for the LSB or MSB.
void TM1638::displayLED8(uint8_t number, bool lsbFirst) {
  uint8_t digit;
  if(_numLEDs > 7) {                                      // We need at least 8 digits to display an 8-bit binary number, leftmost digit is #0.
    for(digit = 0; digit < 8; digit++) {
      if(lsbFirst) {
        // Write the LSB -> MSB bit status to the LED.
        this->displayLED1(digit, ((number >> digit) & 0x01));
      }
      else {
        // Write the MSB -> LSB bit status to the LED.
        this->displayLED1(digit, ((number >> (7 - digit)) & 0x01));
      }
    }
  }
}

// Turn ON/OFF the LED at a specific position.
void TM1638::displayLED1(uint8_t digit, bool status) {
  // Boundry check the digit number, leftmost digit is #0.
  if(_numLEDs > 0 && digit < _numLEDs) {
    bitWrite(_allLEDs, digit, status);
    this->writeCommand(ADDR_FIXED38);                     // Cmd to set specific address mode.
    this->writeDigit(digit, true);                        // Write the status to the specified LED.
  }
}

 // Turn ON/OFF the decimal point in a specific digit.
void TM1638::displayDP(uint8_t digit, bool status) {
  // Boundry check the digit number, leftmost digit is #0.
  if(digit < _numDigits) {
    bitWrite(_registers[digit], 7, status);
    this->writeCommand(ADDR_FIXED38);                     // Cmd to set specific address mode.
    this->writeDigit(digit);                              // Write the digit decimal point to the display.
  }
}

// Read the buttons from 4 bytes (b0 = s1, s2, s3, s4 and b4 = s5, s6, s7, s8) into a single byte.
uint8_t TM1638::readButtons(void) {
	uint8_t counter, buttons = 0;
  if(_numButtons > 0) {
    this->start();                                        // Send the start signal to the TM1638.
    this->writeByte(READ_KEYS38);                         // Cmd to set key scan mode.
    pinMode(_dataPin, INPUT);                             // Set the data pin to be an input.
    for (counter = 0; counter < 4; counter++) {           // Read in 4 bytes of data.
      buttons |= (this->readByte() << counter);           // Get the byte and shift b0 and b4 to the left as appropriate,
                                                          //   and merge the button bits into a single byte.
    }
    pinMode(_dataPin, OUTPUT);                            // Set the data pin back to an output.
    this->stop();                                         // Send the stop signal to the TM1638.
	}
  return buttons;
}


/***************************/
/* Private Class Functions */
/***************************/

// Write a command to the TM1638.
void TM1638::writeCommand(uint8_t command) {
  this->start();                                          // Send the start signal to the TM1638.
  this->writeByte(command);                               // Write the command to the TM1638.
  this->stop();                                           // Send the stop signal to the TM1638.
}

// Write the given logical digit value to the correct physical digit.
void TM1638::writeDigit(uint8_t digit, bool LED) {
  this->start();                                          // Send the start signal to the TM1638.
  if(LED) {
    // Set the address for the requested digit and write the number to the display digit.
    this->writeByte(STARTADDR38 + (_tmDigitMap[digit] << 1) + 1);
    this->writeByte((_allLEDs >> digit) & 0x01);
  }
  else {
    // Set the address for the requested digit and write the number to the display digit.
    this->writeByte(STARTADDR38 + (_tmDigitMap[digit] << 1));
    this->writeByte(_registers[digit]);
  }
  this->stop();                                           // Send the stop signal to the TM1638.
}

// Read a byte of data from the TM1638 - using the Arduino shift function.
uint8_t TM1638::readByte(void) {
  uint8_t data;
  data = shiftIn(_dataPin, _clkPin, LSBFIRST);
  return data;
}
// Write a byte of data to the TM1638 - using the Arduino shift function.
void TM1638::writeByte(uint8_t data) {
  shiftOut(_dataPin, _clkPin, LSBFIRST, data);
}
// Send a start signal to the TM1638 - low level bit banging as per protocol.
void TM1638::start(void) {
  digitalWrite(_stbPin, LOW);
}
// Send a stop signal to the TM1638 - low level bit banging as per protocol.
void TM1638::stop(void) {
  digitalWrite(_stbPin, HIGH);
}

// EOF