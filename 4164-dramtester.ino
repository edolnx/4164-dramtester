

/* 
 *  Originally Written by Noel Llopis in 2021
 *  
 *  Based on initial code by Chris Osborn <fozztexx@fozztexx.com>
 *  http://insentricity.com/a.cl/252
 *  
 *  Heavily modified by Carl Perry in 2022
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>

#define DIN             8
#define DOUT            9
#define CAS             10
#define RAS             11
#define WE              12

#define STATUS    13
#define FAILED    18
#define SUCCESS   19

#define ADDR_BITS 8
#define COLROW_COUNT (1 << ADDR_BITS)

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     17 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Disable Splash Screen
#define SSD1306_NO_SPLASH 1
// Setup Display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write("4164 Tester");
  display.setCursor(0,12);
  display.write(" 00  11  01  10  RR ");
  display.display();
  
  pinMode(DIN, OUTPUT);
  pinMode(DOUT, INPUT);

  pinMode(CAS, OUTPUT);
  pinMode(RAS, OUTPUT);
  pinMode(WE, OUTPUT);

  DDRD = 0xff;
  
  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);
  digitalWrite(WE, HIGH);

  pinMode(STATUS, OUTPUT);
  pinMode(FAILED, OUTPUT);
  pinMode(SUCCESS, OUTPUT);

  digitalWrite(STATUS, HIGH);
  digitalWrite(FAILED, LOW);
  digitalWrite(SUCCESS, LOW);
}

void loop()
{
  display.setCursor(0,12);
  display.write(">00< 11  01  10  RR ");
  display.display();
  checkSame(0);

  display.setCursor(0,12);
  display.write("[00]>11< 01  10  RR ");
  display.display();
  checkSame(1);

  display.setCursor(0,12);
  display.write("[00][11]>01< 10  RR ");
  display.display();
  checkAlternating(0);

  display.setCursor(0,12);
  display.write("[00][11][01]>10< RR ");
  display.display();
  checkAlternating(1);

  display.setCursor(0,12);
  display.write("[00][11][01][10]>RR<");
  display.display();
  checkRandom();
  
  display.setCursor(0,12);
  display.write("[00][11][01][10][RR]");
  digitalWrite(STATUS, LOW);
  display.setCursor(80,0);
  display.write("PASS");
  display.display();

  while (1) {};
}

static inline void writeToRowCol(int row, int col)
{
  PORTD = row;
  digitalWrite(RAS, LOW);
  PORTD = col;
  digitalWrite(CAS, LOW);

  digitalWrite(WE, LOW);

  digitalWrite(WE, HIGH);
 
  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);
}


static inline int readFromRowCol(int row, int col)
{
  PORTD = row;
  digitalWrite(RAS, LOW);
  PORTD = col;
  digitalWrite(CAS, LOW);

  int val = digitalRead(DOUT);
 
  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);
  return val;
}

void fail()
{
  display.setCursor(80,0);
  display.write("FAIL");
  digitalWrite(STATUS, LOW);
  digitalWrite(FAILED, HIGH);
  display.display();

  while (1) {};
}

void checkSame(int val)
{
  digitalWrite(DIN, val);
 
  for (int col=0; col<COLROW_COUNT; col++)
    for (int row=0; row<COLROW_COUNT; row++)
      writeToRowCol(row, col);

  /* Reverse DIN in case DOUT is floating */
  digitalWrite(DIN, !val);
 
  for (int col=0; col<COLROW_COUNT; col++)
    for (int row=0; row<COLROW_COUNT; row++)
      if (readFromRowCol(row, col) != val)
        fail();

  return;
}

void checkAlternating(int start)
{
  int i = start;
  for (int col=0; col<COLROW_COUNT; col++) 
  {
    for (int row=0; row<COLROW_COUNT; row++) 
    {
      digitalWrite(DIN, i);
      i = !i;
      writeToRowCol(row, col);
    }
  }
  
  for (int col=0; col<COLROW_COUNT; col++) 
  {
    for (int row=0; row<COLROW_COUNT; row++) 
    { 
      if (readFromRowCol(row, col) != i)
        fail();
  
      i = !i;
    }
  }
  
  return;
}


void checkRandom()
{
  // Generate a somewhat random seed
  const int seed = analogRead(16);

  randomSeed(seed);
  for (int col=0; col<COLROW_COUNT; col++) 
  {
    for (int row=0; row<COLROW_COUNT; row++) 
    {
      const int value = (int)random(2);    
      digitalWrite(DIN, value);
      writeToRowCol(row, col);
    }
  }

  // Set the same seed as for the write to know what value to get
  randomSeed(seed);
  for (int col=0; col<COLROW_COUNT; col++) 
  {
    for (int row=0; row<COLROW_COUNT; row++) 
    {
      const int value = (int)random(2);    
      if (readFromRowCol(row, col) != value)
        fail();
    }
  }
  
  return;
}
