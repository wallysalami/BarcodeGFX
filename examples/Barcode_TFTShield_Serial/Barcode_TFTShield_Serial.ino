#include <MCUFRIEND_kbv.h>
#include <BarcodeGFX.h>

MCUFRIEND_kbv display;

BarcodeGFX barcode(display);

void setup() {
  Serial.begin(9600);

  int x = 01234;
  Serial.println(x);

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
    String text = Serial.readStringUntil('\n');
    Serial.println(text);

    display.fillScreen(TFT_BLACK);
    
    barcode.setScale(3)
           .setColors(TFT_WHITE, TFT_BLACK);

    // Center the barcode in landscape
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
