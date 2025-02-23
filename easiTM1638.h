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
 * Built on my TM1637 library, and work by Gavin Lyon and Handson Technology.
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
 * *  easiTM1638 Header File  *
 * ****************************
 */

#ifndef __TM1638_H
  #define __TM1638_H
  #include <Arduino.h>

  #define ON          HIGH
  #define OFF         LOW
  
  // Command and address definitions for the TM1638.
  #define ADDR_AUTO       0x40
  #define READ_KEYS       0x42
  #define ADDR_FIXED      0x44
  #define DISP_OFF        0x80
  #define DISP_ON         0x88
  #define STARTADDR       0xc0 

  // The decimal points are controlled via bit 7 of each display digit.
  #define DP_CTRL         0x80

  // Definitions for the 7 segment display brightness.
  #define INTENSITY_MIN   0x00
  #define INTENSITY_TYP   0x02
  #define INTENSITY_MAX   0x07

  // Hardware related definitions.
  #define DEF_TM_CLK      2
  #define DEF_TM_DIN      3
  #define DEF_TM_STB      4
  #define DEF_LEDS        8
  #define MAX_LEDS        8
  #define DEF_DIGITS      8
  #define MAX_DIGITS      8
  #define DEF_BUTTONS     8
  #define MAX_BUTTONS     8

  class TM1638 {
    public:
      // TM1638 Class instantiation.
      TM1638(uint8_t = DEF_TM_STB, uint8_t = DEF_TM_CLK, uint8_t = DEF_TM_DIN);
      uint8_t cmdDispCtrl;                                // The current display control command.
      uint8_t charTableSize;                              // The size of the defined character code table.
      static uint8_t tmCharTable[];                       // This is a class variable, shared across all class instances.
      // Set up the display and initialise it with defaults values - with the default digit map.
      void begin(uint8_t = DEF_BUTTONS, uint8_t = DEF_LEDS, uint8_t = DEF_DIGITS, uint8_t = INTENSITY_TYP);
      // Set up the display and initialise it with defaults values - with a supplied digit map.
      void begin(uint8_t*, uint8_t = DEF_BUTTONS, uint8_t = DEF_LEDS, uint8_t = DEF_DIGITS, uint8_t = INTENSITY_TYP);
      void displayOff(void);                              // Turn the TM1638 display OFF.
      void displayClear(void);                            // Clear all the LEDs and digits (+dps) in the display.
      void displayBrightness(uint8_t = INTENSITY_TYP);    // Set the brightness and turn the TM1638 display ON.
      void displayBin8(uint8_t, bool = false);            // Display a binary integer between 0b00000000 - 0b11111111, starting at digit 0 for the LSB or MSB.
      void displayChar(uint8_t, uint8_t, bool = false);   // Display a character in a specific digit.
      void displayInt8(uint8_t, uint8_t, bool = true);    // Display a decimal integer between 0 - 99, or a hex integer between 0x00 - 0xff, starting at a specific digit.
      void displayInt12(uint8_t, uint16_t, bool = true);  // Display a decimal integer between 0 - 999, or a hex integer between 0x000 - 0xfff, starting at a specific digit.
      void displayInt16(uint8_t, uint16_t, bool = true);  // Display a decimal integer between 0 - 9999, or a hex integer between 0x0000 - 0xffff, starting at a specific digit.
      void displayLED8(uint8_t, bool = false);            // Display a binary integer between 0b00000000 - 0b11111111 on the LEDs, starting at LED 0 for the LSB or MSB.
      void displayLED1(uint8_t, bool = OFF);              // Turn ON/OFF the LED at a specific position.
      void displayDP(uint8_t, bool = OFF);                // Turn ON/OFF the decimal point in a specific digit.
      uint8_t readButtons(void);                          // Read all the buttons into a single byte.
    private:
      uint8_t _clkPin;                                    // The current TM1638 clock pin.
      uint8_t _dataPin;                                   // The current TM1638 data pin.
      uint8_t _stbPin;                                    // The current TM1638 strobe pin.
      uint8_t _numLEDs;                                   // The number of TM1638 module LEDs.
      uint8_t _numDigits;                                 // The number of TM1638 module digits.
      uint8_t _numButtons;                                // The number of TM1638 module buttons.
      uint8_t _brightness;                                // The current TM1638 display brightness.
      uint8_t _allLEDs = 0;                               // A byte used to hold the TM1638 module LED values.
      uint8_t _registers[MAX_DIGITS] = {0};               // An array used to hold the TM1638 display digit values.
      uint8_t* _tmDigitMap;                               // A pointer to the physical to logical digit mapping.
      static uint8_t tmDigitMapDefault[];                 // An array to hold the default physical to logical digit mapping.
      void writeCommand(uint8_t);                         // Write a command to the TM1638.
      void writeDigit(uint8_t, bool = false);             // Write the given logical digit value to the correct physical digit.
      uint8_t readByte(void);                             // Read a byte of data from the TM1638.
      void writeByte(uint8_t);                            // Write a byte of data to the TM1638.
      void start(void);                                   // Send start signal to the TM1638.
      void stop(void);                                    // Send stop signal to the TM1638.
  };
#endif

// EOF