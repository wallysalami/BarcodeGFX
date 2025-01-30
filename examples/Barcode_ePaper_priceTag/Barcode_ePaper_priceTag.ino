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
