#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <BarcodeGFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 21
#define SCL_PIN 22
#define RESET_PIN -1
#define I2C_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, RESET_PIN);

BarcodeGFX barcode(display);

void setup() {
  Serial.begin(9600);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) { 
    Serial.println("SSD1306 allocation failed");
    while (true) {} // Don't proceed, loop forever
  }

  display.clearDisplay();

  // setBackgroundColor is important here
  // WHITE = 0x01, default = 0xFF
  // passing color value 0xFF to Adafruit_SSD1306 causes weird bugs sometimes
  barcode.setBackgroundColor(WHITE).setShowDigits(false).setScale(2);

  bool success = barcode.draw("01234565", 3, 0, 64);
  if (!success) {
    Serial.println("Invalid barcode!");
  }
  else {
    Serial.println("Barcode successfully drawn!");
  }

  display.display();
}

void loop() {
}