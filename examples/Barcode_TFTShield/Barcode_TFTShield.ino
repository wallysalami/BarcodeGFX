#include <MCUFRIEND_kbv.h>
#include <BarcodeGFX.h>

#define SAMPLE_BARCODE_HEIGHT 70

MCUFRIEND_kbv display;

size_t x;

BarcodeGFX barcode(display);

struct BarcodeSample {
  String barcode; // could also be char[], uint64_t or unsigned long long
  BarcodeType type;
  uint16_t backgroundColor;
  uint16_t barColor;
};

BarcodeSample samples[] = {
  {"5000159344074", BarcodeType::EAN13, TFT_WHITE,  TFT_BLACK},
  {"42353720",      BarcodeType::EAN8,  TFT_YELLOW, TFT_BLUE},
  {"034000403622",  BarcodeType::UPCA,  TFT_GREEN,  TFT_PURPLE},
  {"01234565",      BarcodeType::UPCE,  TFT_PINK,   TFT_MAROON},
};

void setup() {
  Serial.begin(9600);

  // Initialize the display
  display.begin(display.readID());
  display.fillScreen(TFT_BLACK);
  display.setRotation(0); // portrait orientation

  // Calculte the positions
  barcode.setShowDigits(true)
         .setScale(2);
  int numberOfSamples = sizeof(samples) / sizeof(BarcodeSample);
  int deltaY = (display.height() - numberOfSamples * SAMPLE_BARCODE_HEIGHT) / (numberOfSamples + 1);
  int y = deltaY;
  int x = (display.width() - barcode.getWidth(BarcodeType::UPCA))/2;

  // Draw sample codes
  for (int i = 0; i < numberOfSamples; i++) {
    barcode.setColors(samples[i].backgroundColor, samples[i].barColor);

    bool success = barcode.draw(samples[i].barcode, x, y, SAMPLE_BARCODE_HEIGHT, samples[i].type);

    if (!success) {
      display.setTextSize(2);
      display.setTextColor(TFT_WHITE);
      display.setCursor(10, y+30);
      display.print("INVALID BARCODE!");
    }

    y += deltaY + SAMPLE_BARCODE_HEIGHT;
  }
}

void loop() {
}
