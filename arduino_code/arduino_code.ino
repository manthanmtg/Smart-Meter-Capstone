int analogPin = A0;
int adcVal = 0; //variable to store the value read
double scaleFactor = 100.0;

#include "EEPROMAnything.h"

// <oled setup>
#include <Adafruit_GFX.h>
#include "Fonts/FreeSerif9pt7b.h"
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI 9 // D1
#define OLED_CLK 10 // D0
#define OLED_DC 11
#define OLED_CS 12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// </oled_setup>

int count = 0;
double usage = 0.0;

void setup()
{
  Serial.begin(9600);
  Serial3.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setFont(&FreeSerif9pt7b);
  //Setup serial
  EEPROM_readAnything(0, usage);
}

String data = "";

void loop()
{
  // display.clearDisplay();
  int peakAdc = 0;
  for (int i = 0; i < 150; ++i)
  {
    adcVal = analogRead(analogPin);
    peakAdc = max(adcVal, peakAdc);
  }
  double vout = (peakAdc / 1023.0) * 5000.0;
  double peakCurrent = (vout - 2500) / scaleFactor;
  double effCurrent = peakCurrent * 0.707;
  effCurrent = effCurrent < 0 ? 0 : effCurrent;
  Serial.print("Current: ");
  Serial.println(effCurrent);
  usage += ((230.0 * effCurrent) / 3600000.0);
  Serial.print("Usage: ");
  Serial.println(usage, 6);

  data = "";
  data += effCurrent;
  data += ";";
  data += usage;

  Serial3.println(data);

  if (Serial3.available())
  {
    String msg = "";
    while (Serial3.available())
    {
      msg += char(Serial3.read());
      delay(50);
    }
    Serial.println(msg);
  }
  //dispLED(String(effCurrent), String(usage));
  dispLED(String(effCurrent), String(usage));
  EEPROM_writeAnything(0, usage);
  delay(1000);
}

void dispLED(String cur, String usg)
{
  display.clearDisplay();      //Eraser any previous display on the screen
  display.setTextSize(1);      //set the size of the text
  display.setTextColor(WHITE); //color setting
  display.setCursor(2, 14);    //The string will start at 10,0 (x,y)
  display.print("Current: " + cur + "A");
  display.setCursor(2, 30); //The string will start at 10,0 (x,y)
  display.print("Count: " + String(count));
  count += 1;
  display.setCursor(2, 46);
  display.print("Usage: " + String(usg) + "KWh");
  display.display(); //send the text to the screen
}