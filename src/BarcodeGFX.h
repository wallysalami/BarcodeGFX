/*************************************************

Copyright © 2025 Jan K. S.
MIT License
https://github.com/wallysalami/BarcodeGFX

*************************************************/

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "BarcodeChecker.h"

class BarcodeGFX {
public:
  BarcodeGFX(
    Adafruit_GFX& _display
  );

  // if BarcodeType is Unknown, it will detect the type
  // if necessary, will automatically add leading zeros
  // returns true and draws the barcode if text is a valid barcode
  // returns false and doesn't draw otherwise
  bool draw(const char *codeText,   int16_t x, int16_t y, uint16_t height, BarcodeType type = BarcodeType::Unknown) const;
  bool draw(const String &codeText, int16_t x, int16_t y, uint16_t height, BarcodeType type = BarcodeType::Unknown) const;
  bool draw(uint64_t codeNumber,    int16_t x, int16_t y, uint16_t height, BarcodeType type = BarcodeType::Unknown) const;

  // Setters / getters for personalized drawing
  BarcodeGFX& setScale(uint16_t _scale);
  uint16_t getScale() const;

  BarcodeGFX& setShowDigits(bool _showDigits);
  bool getShowDigits() const;

  BarcodeGFX& setColors(uint16_t _backgroundColor, uint16_t _barColor);
  BarcodeGFX& setBackgroundColor(uint16_t _backgroundColor);
  uint16_t getBackgroundColor() const;
  BarcodeGFX& setBarColor(uint16_t _barColor);
  uint16_t getBarColor() const;
  
  BarcodeGFX& setPadWithLeadingZeros(bool padWithLeadingZeros);
  bool getPadWithLeadingZeros() const;

  // width is determined by the scale property and the barcode type
  // this method is usefull if you need to automatically center the drawing on screen
  // in that case, you can make x = (screenWidth - barcodeWidth) / 2
  uint16_t getWidth(const char *codeText) const;
  uint16_t getWidth(const String &codeText) const;
  uint16_t getWidth(uint64_t codeNumber) const;
  uint16_t getWidth(BarcodeType type) const;


private:
  Adafruit_GFX& display;
  uint16_t scale = 1;
  bool showDigits = true;
  uint16_t backgroundColor = 0xFFFF;
  uint16_t barColor = 0x0000;
  bool padWithLeadingZeros = true;

  void drawPattern(const char *pattern, int16_t x, int16_t y, int16_t barHeight) const;

  void getDigitSize(uint16_t& width, uint16_t& height) const;
  uint16_t getDigitWidth() const;
  uint16_t getDigitHeight() const;

  void drawDigit(char digit, int16_t x, int16_t y) const;
};