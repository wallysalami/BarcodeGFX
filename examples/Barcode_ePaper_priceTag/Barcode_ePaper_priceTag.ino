/*************************************************

Copyright © 2025 Jan K. S.
MIT License
https://github.com/wallysalami/BarcodeGFX

This example simulates a price tag with a Barcode.
It combines the QR Code with other drawing methods from Adafruit_GFX.

This program was tested on a ESP32 S3 with a WeAct 2.9'' Black-White E-Paper Display.
It should work with Waveshare E-Paper Display as well.
That display uses the SPI protocol to receive drawing commands.
The SPI pins changes from microcontroller to microcontroller.
Here are some examples:

Microcontroller | SDA (MOSI) pin | SCL (SCK) pin  
--------------------------------------------------
Arduino Uno     |       12       |       13       
Arduino Mega    |       51       |       52       
ESP32           |       23       |       18       
ESP32 S3        |       11       |       12       

You must also install the GxEPD2 library to compile it!

*************************************************/

#include <GxEPD2_BW.h>
#include <BarcodeGFX.h>

#define CS_PIN 10
#define DC_PIN 14
#define RESET_PIN 15
#define BUSY_PIN 16

GxEPD2_290_T94_V2 configuration(CS_PIN, DC_PIN, RESET_PIN, BUSY_PIN);
GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(configuration);

BarcodeGFX barcode(display);

void setup() {
  // Initialize the display
  display.init();

  // Prepare the drawing
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(1);

  display.setCursor(10, 0);
  display.setTextSize(2);
  display.print("Snickers 45g");

  display.setTextSize(4);
  display.setCursor(10, 25);
  display.print("R$ 4,50");

  barcode.setScale(2);
  barcode.draw("7896423420180", 0, 55, 80);

  // Update display
  display.display(false);
}

void loop() {
}
