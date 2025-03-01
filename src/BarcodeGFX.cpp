/*************************************************

Copyright © 2025 Jan K. S.
MIT License
https://github.com/wallysalami/BarcodeGFX

*************************************************/

#include "BarcodeGFX.h"

#define getBit(value, bit) ((value >> bit) & 1)

// EAN-13 encoding patterns for digits 0-9
static constexpr byte EAN_L[10] = {
  0b0001101, 0b0011001, 0b0010011, 0b0111101, 0b0100011, 0b0110001, 0b0101111, 0b0111011, 0b0110111, 0b0001011
};

static constexpr byte EAN_G[10]= {
  0b0100111, 0b0110011, 0b0011011, 0b0100001, 0b0011101, 0b0111001, 0b0000101, 0b0010001, 0b0001001, 0b0010111
};

// EAN-13 Encoding pattern according to the first digit (determines the L and G combination for the left side)
// L = 0, G = 1
static constexpr byte EAN13_PATTERN[10] = {
  0b000000, 0b001011, 0b001101, 0b001110, 0b010011, 0b011001, 0b011100, 0b010101, 0b010110, 0b011010
};

// UPC-E encoding pattern according to the first and last digit
// O = 0, E = 1
static constexpr byte UPCE_ZERO_PATTERN[10] = {
  0b111000, 0b110100, 0b110010, 0b110001, 0b101100, 0b100110, 0b100011, 0b101010, 0b101001, 0b100101 
};

static constexpr int PADDING = 5;
static constexpr int DIGIT_PADDING_TOP = 3;
static constexpr int MAX_BARCODE_LENGTH = 20; // max digits in a 64 bit number


void uint64ToStr(uint64_t number, char *text);


BarcodeGFX::BarcodeGFX(Adafruit_GFX& _display)
  : display(_display) {
}

/////////////////////////////////////////////////
// draw methods
/////////////////////////////////////////////////

bool BarcodeGFX::draw(const String &codeText, int16_t x, int16_t y, uint16_t height,
                      BarcodeType type) const {
  return draw(codeText.c_str(), x, y, height, type);
}

bool BarcodeGFX::draw(uint64_t codeNumber, int16_t x, int16_t y, uint16_t height,
                      BarcodeType type) const {
  char codeText[MAX_BARCODE_LENGTH + 1];
  BarcodeHelper::uint64ToStr(codeNumber, codeText);
  
  return draw(codeText, x, y, height, type);
}

bool BarcodeGFX::draw(const char *codeText, int16_t x, int16_t y, uint16_t height,
                      BarcodeType barcodeType) const {
  if (barcodeType == BarcodeType::Unknown) {
    barcodeType = BarcodeChecker::detectType(codeText, padWithLeadingZeros);
    if (barcodeType == BarcodeType::Unknown) {
      return false;
    }
  }

  if (padWithLeadingZeros) {
    char newCodeText[MAX_BARCODE_LENGTH + 1];
    BarcodeChecker::padWithLeadingZeros(newCodeText, codeText, barcodeType);
    codeText = newCodeText;
  }

  if (!BarcodeChecker::isValid(codeText, barcodeType, false)) {
    return false;
  }
  
  int width = getWidth(barcodeType);

  display.fillRect(x, y, width, height, backgroundColor);

  // Adjust drawing parameters
  int padding = PADDING * scale;
  int barY = y + padding;
  int digitHeight = getDigitHeight();
  int numberY = y + height - padding - digitHeight;
  int barHeight = numberY - barY - DIGIT_PADDING_TOP * scale;
  int longBarHeight = barHeight + DIGIT_PADDING_TOP * scale + digitHeight/2;
  if (!showDigits) {
    barHeight += DIGIT_PADDING_TOP * scale;
    longBarHeight = barHeight;
  }
  int numberWidth = getDigitWidth();

  int16_t currentX = x + padding;
  y += padding;

  int index1, index2, index3;
  if (barcodeType == BarcodeType::EAN13) {
    index1 = 1; index2 = 7; index3 = 13;
  }
  else if (barcodeType == BarcodeType::EAN8) {
    index1 = 0; index2 = 4; index3 = 8;
  }
  else if (barcodeType == BarcodeType::UPCA) {
    index1 = 1; index2 = 6; index3 = 11;
  }
  else if (barcodeType == BarcodeType::UPCE) {
    index1 = 1; index2 = 7; index3 = 7;
  }

  // Draw first digit outside (except for EAN-8)
  if (barcodeType != BarcodeType::EAN8 && showDigits) {
    drawDigit(codeText[0], currentX, numberY);
    currentX += numberWidth;
  }

  // Draw start guard pattern
  drawPattern(0b101, 3, currentX, y, longBarHeight);
  currentX += 3 * scale;

  // UPC-A draws long bars for first digit after start guard
  int firstDigit = codeText[0] - '0';
  if (barcodeType == BarcodeType::UPCA) {
    drawPattern(EAN_L[firstDigit], 7, currentX, y, longBarHeight);
    currentX += 7 * scale;
  }

  // Draw left side
  for (int i = index1; i < index2; i++) {
    int digit = codeText[i] - '0';
    // EAN-13 uses a L/G pattern according to the first digit
    byte digitPattern;
    if (barcodeType == BarcodeType::EAN13) {
      byte pattern = EAN13_PATTERN[firstDigit];
      if (getBit(pattern, 6-i) == 1) {
        digitPattern = EAN_G[digit];
      }
      else {
        digitPattern = EAN_L[digit];
      }
    }
    // UPC-E uses a L/G pattern according to the first and the last digit
    else if (barcodeType == BarcodeType::UPCE) {
      int lastDigit = codeText[7] - '0';
      byte pattern = (firstDigit == 0) ? UPCE_ZERO_PATTERN[lastDigit] : ~UPCE_ZERO_PATTERN[lastDigit];
      if (getBit(pattern, 6 - i) == 1) {
        digitPattern = EAN_G[digit];
      }
      else {
        digitPattern = EAN_L[digit];
      }
    }
    else {
      digitPattern = EAN_L[digit];
    }
    drawPattern(digitPattern, 7, currentX, y, barHeight);
    drawDigit(codeText[i], currentX + scale * 1.5, numberY);
    currentX += 7 * scale;
  }

  // Draw middle guard pattern
  if (barcodeType != BarcodeType::UPCE) {
    drawPattern(0b01010, 5, currentX, y, longBarHeight);
    currentX += 5 * scale;
  }

  // Draw right side
  for (int i = index2; i < index3; i++) {
    int digit = codeText[i] - '0';
    drawPattern(~EAN_L[digit], 7, currentX, y, barHeight);
    drawDigit(codeText[i], currentX + scale * 0.5, numberY);
    currentX += 7 * scale;
  }

  // UPC-A draws long bars for last digit before end guard
  if (barcodeType == BarcodeType::UPCA) {
    int lastDigit = codeText[11] - '0';
    drawPattern(~EAN_L[lastDigit], 7, currentX, y, longBarHeight);
    currentX += 7 * scale;
  }

  // Draw end guard pattern
  if (barcodeType == BarcodeType::UPCE) {
    drawPattern(0b010101, 6, currentX, y, longBarHeight);
    currentX += 6 * scale;
  }
  else {
    drawPattern(0b101, 3, currentX, y, longBarHeight);
    currentX += 3 * scale;  
  }
  
  // Draw last digit
  if (barcodeType == BarcodeType::UPCA || barcodeType == BarcodeType::UPCE) {
    drawDigit(codeText[index3], currentX + scale, numberY);
  }

  return true;  
}

/////////////////////////////////////////////////
// Setters / getters
/////////////////////////////////////////////////

BarcodeGFX& BarcodeGFX::setScale(uint16_t _scale) {
  static constexpr uint16_t MAX_SAFE_SCALE = 20;
    
  if (_scale == 0) {
    scale = 1;
  } else if (_scale > MAX_SAFE_SCALE) {
    scale = MAX_SAFE_SCALE;
  } else {
    scale = _scale;
  }
  return *this;
}
uint16_t BarcodeGFX::getScale() const {
  return scale;
}

BarcodeGFX& BarcodeGFX::setShowDigits(bool _showDigits) {
  showDigits = _showDigits;
  return *this;
}
bool BarcodeGFX::getShowDigits() const {
  return showDigits;
}

BarcodeGFX& BarcodeGFX::setColors(uint16_t _backgroundColor, uint16_t _barColor) {
  backgroundColor = _backgroundColor;
  barColor = _barColor;
  return *this;
}
BarcodeGFX& BarcodeGFX::setBarColor(uint16_t _barColor) {
  barColor = _barColor;
  return *this;
}
uint16_t BarcodeGFX::getBarColor() const {
  return barColor;
}
BarcodeGFX& BarcodeGFX::setBackgroundColor(uint16_t _backgroundColor) {
  backgroundColor = _backgroundColor;
  return *this;
}
uint16_t BarcodeGFX::getBackgroundColor() const {
  return backgroundColor;
}

BarcodeGFX& BarcodeGFX::setPadWithLeadingZeros(bool _padWithLeadingZeros) {
  padWithLeadingZeros = _padWithLeadingZeros;
  return *this;
}
bool BarcodeGFX::getPadWithLeadingZeros() const {
  return padWithLeadingZeros;
}

/////////////////////////////////////////////////
// getWidth (pixels) methods
/////////////////////////////////////////////////

uint16_t BarcodeGFX::getWidth(const String &codeText) const {
  return getWidth(codeText.c_str());
}

uint16_t BarcodeGFX::getWidth(uint64_t codeNumber) const {
  char codeText[MAX_BARCODE_LENGTH + 1];
  uint64ToStr(codeNumber, codeText);
  return getWidth(codeText);
}

uint16_t BarcodeGFX::getWidth(const char *codeText) const {
  BarcodeType type = BarcodeChecker::detectType(codeText);
  return getWidth(type);
}

uint16_t BarcodeGFX::getWidth(BarcodeType type) const {
  int digitWidth = getDigitWidth();

  int padding = 2 * PADDING * scale;
  int guardLength;
  int barLength;

  switch(type){
    case BarcodeType::EAN8:
      guardLength = (3 + 5 + 3) * scale;
      barLength = 7 * 8 * scale;
      break;
    case BarcodeType::EAN13:
      guardLength = (3 + 5 + 3) * scale;
      barLength = 7 * 12 * scale;
      if (showDigits) {
        padding += digitWidth + scale;
      }
      break;
    case BarcodeType::UPCA:
      guardLength = (3 + 5 + 3) * scale;
      barLength = 7 * 12 * scale;
      if (showDigits) {
        padding += 2 * (digitWidth + scale);
      }
      break;
    case BarcodeType::UPCE:
      guardLength = (3 + 0 + 6) * scale;
      barLength = 7 * 6 * scale;
      if (showDigits) {
        padding += 2 * (digitWidth + scale);
      }
      break;
    case BarcodeType::Unknown:
      return 0;
  }
  return padding + guardLength + barLength;
}

/////////////////////////////////////////////////
// Private methods
/////////////////////////////////////////////////

void BarcodeGFX::drawPattern(byte pattern, uint8_t size, int16_t x, int16_t y, int16_t barHeight) const {
  for (int i = 0; i < size; i++) {
    if (getBit(pattern, size - i - 1) == 1) {
      display.fillRect(x + i * scale, y, scale, barHeight, barColor);
    }
  }
}

void BarcodeGFX::getDigitSize(uint16_t& width, uint16_t& height) const {
  if (!showDigits) {
    width = 0;
    height = 0;
    return;
  }
  int16_t x, y;
  display.setTextSize(scale);
  display.getTextBounds("0", 0, 0, &x, &y, &width, &height);
}

uint16_t BarcodeGFX::getDigitWidth() const {
  uint16_t width, height;
  getDigitSize(width, height);
  return width;
}

uint16_t BarcodeGFX::getDigitHeight() const {
  uint16_t width, height;
  getDigitSize(width, height);
  return height;
}

void BarcodeGFX::drawDigit(char digit, int16_t x, int16_t y) const {
  if (showDigits) {
    display.setTextSize(scale);
    display.setTextColor(barColor);
    display.setCursor(x, y);
    display.setTextWrap(false);
    display.print(digit);  
  }
}