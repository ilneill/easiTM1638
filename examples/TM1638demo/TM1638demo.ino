/*!
 * TM1638 Example with a TM1638 based 8-digit (+dps) LED display module.
 *
 * Written for the Arduino Uno/Nano/Mega.
 * (c) Ian Neill 2025
 * GPL(v3) Licence
 *
 * Built on my TM1637 library demo for the TM1637 based LED module.
 *
 * ****************************
 * *  easiTM1638 Demo Sketch  *
 * ****************************
 */

#include "easiTM1638.h"

// Optimisation - Enable this to allow the F() macro to keep constant strings in flash, and out of RAM.
#define USE_FLASH
#ifdef USE_FLASH
  #define FLASHSTR(x)       F(x)                          // Substitute with the F() macro.
#else
  #define FLASHSTR(x)       (x)                           // Substitute with just the original string.
#endif

#define ON          HIGH
#define OFF         LOW

// Pin definitions for the TM1638 - the interface might look like I2C, but it is not!
#define CLKPIN      2                                     // Clock.
#define DIOPIN      3                                     // Data Out.
#define STBPIN      4                                     // Strobe.
#define LEDPIN      13                                    // The builtin LED.

// The number of LEDs, Digits and Buttons in the TM1638 based LED display.
#define NUMLEDS     8
#define NUMDIGITS   8
#define NUMBUTTONS  8

// A table to describe the physical to logical digit numbering.
// This can be determined the when this demo first runs with the default digit map.
//uint8_t tmDigitMap[NUMDIGITS] = {0, 1, 2, 3, 4, 5, 6, 7};

// Instantiate a TM1638 display.
TM1638 myDisplay(STBPIN, CLKPIN, DIOPIN);                 // Set clock and data pins.

void setup() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, OFF);
  Serial.begin(9600);
  // TM1638 fixed addressing mode using the default digit map.
  // Buttons = 8, LEDs = 8, Digits = 8, Brightness = 2, Display cleared (all LEDs OFF, segments OFF, and decimal points OFF).
  myDisplay.begin(NUMBUTTONS, NUMLEDS, NUMDIGITS, INTENSITY_TYP);
  // TM1638 fixed addressing mode using a supplied digit map.
  // Buttons = 8, LEDs = 8, Digits = 8, Brightness = 2, Display cleared (all LEDs OFF, segments OFF, and decimal points OFF).
  //myDisplay.begin(tmDigitMap, NUMBUTTONS, NUMLEDS, NUMDIGITS, INTENSITY_TYP);
  Serial.println(FLASHSTR("\nDisplay physical to logical mapping test."));
  findDigitMap();
  Serial.println(FLASHSTR("\nDisplay brightness and LED/digit/button test."));
  testDisplay();
  blinkLED(100);
  delay(1000);
}

void loop() {
  unsigned long timeNow;                                  // Used to time the 10 and 20 min delay demos.
  
  // 8-bit hex number count up, 0x00 - 0xFF, 1 count/250ms.
  Serial.println(FLASHSTR("Demo 1: 8-bit hex count up."));
  countHex8(250);
  blinkLED(100);
  delay(1000);
  
  // 12-bit hex number count up, 0x000 - 0xFFF, 1 count/125ms.
  Serial.println(FLASHSTR("Demo 2: 12-bit hex count up."));
  countHex12(125);
  blinkLED(100);
  delay(1000);
  
  // 16-bit hex number count up, 0x0000 - 0xFFFF, 1 count/125ms.
  Serial.println(FLASHSTR("Demo 3: 16-bit hex count up."));
  countHex16(125);
  blinkLED(100);
  delay(1000);
  
  // Decimal number count up, 0 - 9999, 1 count/100ms.
  Serial.println(FLASHSTR("Demo 4: 9999 decimal count up."));
  countUp(9999, 100);
  blinkLED(100);
  delay(1000);
  
  // Decimal number count down, 9999 - 0, 1 count/500ms.
  Serial.println(FLASHSTR("Demo 5: 9999 decimal count down."));
  countDown(9999, 500);
  blinkLED(100);
  delay(1000);
  
  // A 10 minute timer.
  Serial.print(FLASHSTR("Demo 6: A 10 minute delay() timer: "));
  timeNow = millis();
  countXMins(10);
  Serial.print(millis() - timeNow);
  Serial.println(FLASHSTR("ms"));
  blinkLED(100);
  delay(1000);
  
  // A 20 minute timer with flashing decimal point.
  Serial.print(FLASHSTR("Demo 7: A 20 minute millis() timer & flashing decimal point: "));
  timeNow = millis();
  countXMinsDP(20);
  Serial.print(millis() - timeNow);
  Serial.println(FLASHSTR("ms"));
  blinkLED(100);
  delay(1000);
  
  // A button demo - buttons drive the LEDs display digits.
  Serial.print(FLASHSTR("Demo 8: Press the LED&KEY module buttons."));
  buttonMonitor();
  blinkLED(100);
  delay(1000);
}

// Flash the builtin LED to signal the end of a demo stage.
void blinkLED(uint32_t interval) {
  digitalWrite(LEDPIN, ON);
  delay(interval);
  digitalWrite(LEDPIN, OFF);
}

void findDigitMap() {
  byte digit, counter;
  // Map the digits, physical to logical.
  for(digit = 0; digit < NUMDIGITS; digit++) {
    myDisplay.displayChar(digit, digit);
    delay(1000);
  }
  // Pulse the LEDs and decimal points as a bar, giving some time to note down the number on the TM1638 based LED display.
  for(counter = 0; counter < NUMDIGITS; counter++) {
    for(digit = 0; digit < NUMDIGITS; digit++) {
      myDisplay.displayLED1(digit, ON);
      myDisplay.displayDP(digit, ON);
      delay(100);
    }
    for(digit = 0; digit < NUMDIGITS; digit++) {
      myDisplay.displayLED1((NUMDIGITS - digit - 1), OFF);
      myDisplay.displayDP((NUMDIGITS - digit - 1), OFF);
      delay(100);
    }
  }
  // Clear the display as we leave the digit mapping function.
  myDisplay.displayClear();
}

void testDisplay() {
  byte counter, brightness, character;
  // LEDs ON/OFF test.
  myDisplay.displayClear();
  for(counter = 0; counter < NUMLEDS; counter++) {
    myDisplay.displayLED1(counter, ON);
    delay(500);
    myDisplay.displayLED1(counter, OFF);
    delay(500);
  }
  // Display brightness test.
  for(brightness = INTENSITY_MIN; brightness <= INTENSITY_MAX; brightness++) {
    myDisplay.displayBrightness(brightness);
    for(counter = 0; counter < NUMDIGITS; counter++) {
      myDisplay.displayChar(counter, brightness);
    }
    delay(1000);
  }
  // Clear the display and set the brightness to the typical value.
  myDisplay.displayClear();
  myDisplay.displayBrightness(INTENSITY_TYP);
  // Display all characters on each digit.
  for(counter = 0; counter < NUMDIGITS; counter++) {
    // Cycle through each code in the character table, deliberately exceeding the table size by 1 to finish on a default space (0x00).
    for(character = 0; character <= myDisplay.charTableSize; character++) {
      myDisplay.displayChar(counter, character);
      delay(200);
    }
  }
  // Decimal points ON/OFF test.
  myDisplay.displayClear();
  for(counter = 0; counter < NUMDIGITS; counter++) {
    myDisplay.displayDP(counter, ON);
    delay(500);
    myDisplay.displayDP(counter, OFF);
    delay(500);
  }
  // ALL LEDs, Segments, and decimal points ON.
  myDisplay.displayClear();
  for(counter = 0; counter < max(NUMDIGITS, NUMLEDS); counter++) {
    myDisplay.displayLED1(counter, ON);
    myDisplay.displayChar(counter, 0x08);
    myDisplay.displayDP(counter, ON);
  }
  delay(2000);
  // Clear the LEDs and display (+dps) as we leave the LED/Digit/Button test function.
  myDisplay.displayClear();
}

void countHex8(uint32_t interval) {
  byte counter = 0;
  myDisplay.displayChar(0, 0x12);                         // Print an "h" in the 1st digit.
  myDisplay.displayChar(4, 0x12);                         // Print an "h" in the 5th digit.
  do {
    myDisplay.displayLED8(counter);                       // Display the 8-bit count on the LEDs.
    myDisplay.displayInt8(1, counter, false);             // Print the 8-bit count in the 2nd and 3rd digits.
    myDisplay.displayInt8(5, counter, false);             // Print the 8-bit count in the 6th and 7th digits.
    circle8s(3, counter);                                 // Display one circulating segment in 4th digit.
    circle8ss(7, counter);                                // Display two circulating segments in 8th digit.
    delay(interval);
  } while(++counter != 0);                                // The counter is an 8-bit number that will wrap to zero.
  // Clear the LEDs and display (+dps) as we leave the 8-bit counter function.
  myDisplay.displayClear();
}

void countHex12(uint32_t interval) {
  uint16_t counter = 0;
  myDisplay.displayChar(0, 0x12);                         // Print an "h" in the 1st digit.
  myDisplay.displayChar(4, 0x12);                         // Print an "h" in the 5th digit.
  do {
    myDisplay.displayInt12(1, counter, false);            // Print the 12-bit count in the 1st, 2nd and 3rd digits.
    myDisplay.displayInt12(5, counter, false);            // Print the 12-bit count in the 6th, 7th and 8th digits.
    delay(interval);
  } while(++counter != 0x1000);                           // The counter is a 16-bit number, so watch for it crossing the 12-bit boundry.
}

void countHex16(uint32_t interval) {
  uint16_t counter = 0;
  do {
    myDisplay.displayInt16(0, counter, false);            // Print the 16-bit count in the 1st, 2nd, 3rd and 4th digits.
    myDisplay.displayInt16(4, counter, false);            // Print the 16-bit count in the 5th, 6th, 7th and 8th digits.
    delay(interval);
  } while(++counter != 0);                                // The counter is a 16-bit number that will wrap to zero.
}

void countUp(uint16_t number, uint32_t interval) {
  int16_t counter;
  for(counter = 0; counter <= number; counter++) {
    myDisplay.displayInt16(0, counter);                   // Print the 0 - 9999 count in the 1st, 2nd, 3rd and 4th digits.
    myDisplay.displayInt16(4, counter);                   // Print the 0 - 9999 count in the 5th, 6th, 7th and 8th digits.
    delay(interval);
  }
}

void countDown(uint16_t number, uint32_t interval) {
  int16_t counter;
  for(counter = number; counter >= 0; counter--) {
    myDisplay.displayInt16(0, counter);                   // Print the 9999 - 0 count in the 1st, 2nd, 3rd and 4th digits.
    myDisplay.displayInt16(4, counter);                   // Print the 9999 - 0 count in the 5th, 6th, 7th and 8th digits.
    delay(interval);
  }
}

void countXMins(byte minutesMax) {
  byte minutes, seconds;
  if(minutesMax > 100) {                                  // Clip the number at the maximum for a 4-digit MM.SS timer.
    minutesMax = 100;
  }
  for(minutes = 0; minutes < minutesMax; minutes++) {
    for(seconds = 0; seconds < 60; seconds++) {
      myDisplay.displayChar(0, minutes / 10);             // Print the minutes (x10) in the 1st digit.
      myDisplay.displayChar(1, minutes % 10);             // Print the minutes (units) in the 2nd digit.
      myDisplay.displayChar(2, seconds / 10);             // Print the seconds (x10) in the 3rd digit.
      myDisplay.displayChar(3, seconds % 10);             // Print the seconds (units) in the 4th digit.
      myDisplay.displayChar(4, minutes / 10);             // Print the minutes (x10) in the 5th digit.
      myDisplay.displayChar(5, minutes % 10);             // Print the minutes (units) in the 6th digit.
      myDisplay.displayChar(6, seconds / 10);             // Print the seconds (x10) in the 7th digit.
      myDisplay.displayChar(7, seconds % 10);             // Print the seconds (units) in the 8th digit.
      delay(1000);
    }
  }
}

void countXMinsDP(byte minutesMax) {
  bool dPoint = ON;
  byte minutes = 0, seconds = 0;
  unsigned long timeNow, timeMark;
  timeMark = millis();
  if(minutesMax > 100) {                                  // Clip the number at the maximum for a 4-digit MM.SS timer.
    minutesMax = 100;
  }
  while(minutes != minutesMax || !dPoint) {
    timeNow = millis();
    if(timeNow - timeMark >= 500) {
      timeMark = timeNow;
      // Update the display decimal points.
      myDisplay.displayDP(1, dPoint);
      myDisplay.displayDP(5, dPoint);
      // Update the display and increment the time.
      if(dPoint) {
        myDisplay.displayChar(0, minutes / 10);           // Print the minutes (x10) in the 1st digit.
        myDisplay.displayChar(1, minutes % 10);           // Print the minutes (units) in the 2nd digit.
        myDisplay.displayChar(2, seconds / 10);           // Print the seconds (x10) in the 3rd digit.
        myDisplay.displayChar(3, seconds % 10);           // Print the seconds (units) in the 4th digit.
        myDisplay.displayChar(4, minutes / 10);           // Print the minutes (x10) in the 5th digit.
        myDisplay.displayChar(5, minutes % 10);           // Print the minutes (units) in the 6th digit.
        myDisplay.displayChar(6, seconds / 10);           // Print the seconds (x10) in the 7th digit.
        myDisplay.displayChar(7, seconds % 10);           // Print the seconds (units) in the 8th digit.
        if(++seconds == 60) {
          seconds = 0;
          minutes++;
        }
      }
      // Toggle the decimal point flag.
      dPoint = !dPoint;
    }
  };
}

void buttonMonitor() {
  byte buttons;
  unsigned long timeNow = millis();
  while(millis() - timeNow < 30000) {
    buttons = myDisplay.readButtons();                    // Read all the buttons in one go.
    myDisplay.displayBin8(buttons, true);                 // Print the 00000000 - 11111111 button value to the display digits.
    myDisplay.displayLED8(buttons, true);                 // Display the 00000000 - 11111111 button value to the LEDs.
    delay(10);
  };
  myDisplay.displayClear();
}

// Display a circulating segment in a digit - a b g e d c g f
void circle8s(byte digit, byte number) {
  byte circleSegments[] = {0x01, 0x02, 0x40, 0x10, 0x08, 0x04, 0x40, 0x20};
  // Ensure we have a number 0 - 7, and raw print that segment bitmap on the selected display digit.
  myDisplay.displayChar(digit, circleSegments[(number & 0x07)], true);
}

// Display circulating segments in a digit - fa ab bg ge ed dc cg gf
void circle8ss(byte digit, byte number) {
  byte circleSegments[] = {0x21, 0x03, 0x42, 0x50, 0x18, 0x0c, 0x44, 0x60};
  // Ensure we have a number 0 - 7, and raw print that segment bitmap on the selected display digit.
  myDisplay.displayChar(digit, circleSegments[(number & 0x07)], true);
}

// EOF