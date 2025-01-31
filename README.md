# BarcodeGFX

BarcodeGFX is an Arduino library for drawing barcode labels on displays. It is designed to be display-agnostic and works with any library that inherits from Adafruit_GFX.

![Image](https://github.com/user-attachments/assets/a41831ba-21a0-415d-a249-0d6c5ac12af6)
*Example of barcodes drawn on different displays*

## Features
- Support for multiple barcode formats:
  - EAN-13
  - EAN-8
  - UPC-A
  - UPC-E
- Works with any display compatible with Adafruit_GFX library:
  - Adafruit_SSD1306 (monochrome OLED)
  - Adafruit_ILI9341 (TFT LCD SPI)
  - MCUFRIEND_kbv (TFT LCD)
  - GxEPD2 (E-Paper)
  - And many others
- Smart barcode handling:
  - Automatic type detection
  - Checksum validation
  - Leading zeros auto-padding
- Customizable appearance:
  - Adjustable scale (size)
  - Configurable colors
  - Optional numerical digits display

## Installation

1. In the Arduino IDE, go to Sketch -> Include Library -> Manage Libraries...
2. Search for BarcodeGFX
3. Click Install

Or if you want to do it manually:

1. Download the ZIP file of this repository
2. In the Arduino IDE, go to Sketch -> Include Library -> Add .ZIP Library
3. Select the downloaded ZIP file
4. Restart the Arduino IDE

## Usage

### Basic Example
```cpp
#include <BarcodeGFX.h>
#include <MCUFRIEND_kbv.h>

// Initialize your display (example with LCD TFT Shield)
MCUFRIEND_kbv display;

// Create barcode drawer instance
BarcodeGFX barcode(display);

void setup() {
  // Initialize your display first
  display.begin(display.readID());
  display.fillScreen(TFT_BLACK);
  
  // Draw a small black and white barcode
  // Parameters:
  //   barcode: number or text with digits
  //   x: horizontal position (upper left corner)
  //   y: vertical position (upper left corner)
  //   height: desired height in pixels (width is calculated automatically)
  barcode.draw("5000159344074", 0, 0, 70);
}

void loop() {
}
```

### Customized Drawing
```cpp
// Configure appearance using method chaining
barcode.setScale(2) // from 1 to 20
       .setShowDigits(false)
       .setColors(TFT_YELLOW, TFT_BLUE) // background, bars
       .setPadWithLeadingZeros(false);

// Force UPC-A format and check the result
String upcaExample = "034000403622";
bool success = barcode.draw(upcaExample, 0, 0, 100, BarcodeType::UPCA);
if (!success) {
  // Invalid barcode! Nothing has been drawn :(
}
```

### Centering Barcode

```cpp
char ean13Example[] = "5000159344074";

display.setRotation(1); // landscape orientation
barcode.setScale(2);

// Get barcode width to center it
// This call automatically detects the barcode type (EAN-13 in this case)
uint16_t barcodeWidth = barcode.getWidth(ean13Example);
int16_t x = (display.width() - barcodeWidth) / 2;

barcode.draw(ean13Example, x, 0, 100);
```

### Barcode Validation

Barcode digits are always validated before drawing, but you can do it manually too.

By the way, you can pass the digits as 64-bit integers, `char*` or `String` in all methods. But text is better to store leading zeros (e.g., "**0**1234565").

Friendly reminder: don't put leading zeros on integer variables! C/C++ will see them as octal base instead of decimal base (e.g., `int x = 01234;` gets converted to `668`).

```cpp
// Using static validation methods
if (BarcodeChecker::isValid(5000159344074)) {
  // Valid EAN-13 code
}

// Get barcode type
BarcodeType type = BarcodeChecker::detectType("5000159344074");
if (type == BarcodeType::EAN13) {
  // It's an EAN-13 barcode
}

// Validate specific type without 
if (BarcodeChecker::isValid("01234565", BarcodeType::EAN8)) {
  // Valid EAN-8 code
}

// Add missing leading zeros
// First result is "034000403622" (12 digits) and second one is "0034000403622" (13 digits)
String upcaBarcode = BarcodeChecker::padWithLeadingZeros(34000403622, BarcodeType::UPCA);
String ean13Barcode = BarcodeChecker::padWithLeadingZeros(34000403622, BarcodeType::EAN13);

```

Check out more examples in the `examples` folder.

## Contributing

Feel free to open issues and submit pull requests. All contributions are welcome!

## License

This library is released under the MIT License. See the LICENSE file for details.
