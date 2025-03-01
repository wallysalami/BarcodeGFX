/*************************************************

Copyright © 2025 Jan K. S.
MIT License
https://github.com/wallysalami/BarcodeGFX

This example will ask the user to input some text on Serial Monitor.
Then it will draw a Barcode for that text at the center of the screen.
It's an good example of Barcode generation from an arbitrary text.

This program was tested on a Arduino Uno / Mega with a TFT Shield.

You must also install the MCUFRIEND_kbv library to compile it!

*************************************************/

#include <MCUFRIEND_kbv.h>
#include <BarcodeGFX.h>

MCUFRIEND_kbv display;

BarcodeGFX barcode(display);

void setup() {
  Serial.begin(9600);

  // Initialize the display
  display.begin(display.readID());
  display.fillScreen(TFT_BLACK);
  display.setRotation(1); // landscape orientation

  // Write instructions on the screen
  display.setTextSize(3);
  display.setTextColor(TFT_WHITE);
  display.setCursor(15, 90);
  display.print("Enter barcode in");
  display.setCursor(8, 120);
  display.print("Serial Monitor...");
}

void loop() {
  // Draw a big barcode label if user sends a barcode via Serial
  if (Serial.available()) {
    // Get text sent by user
    String text = Serial.readStringUntil('\n');
    Serial.println(text);

    // Erase previous barcode
    display.fillScreen(TFT_BLACK);
    
    // Center the barcode in landscape
    barcode.setScale(3);
    int width = barcode.getWidth(text);
    int height = 150;
    int x = (display.width() - width) / 2;
    int y = (display.height() - height) / 2;

    // Draw it!
    bool success = barcode.draw(text, x, y, height);
    if (!success) {
      display.setTextSize(3);
      display.setTextColor(TFT_WHITE);
      display.setCursor(18, 110);
      display.print("INVALID BARCODE!");
    }
  }
}
