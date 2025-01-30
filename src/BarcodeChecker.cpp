#include "BarcodeChecker.h"

static constexpr int MAX_BARCODE_LENGTH = 20;  // max digits in a 64 bit number

/////////////////////////////////////////////////
// isValid methods
/////////////////////////////////////////////////

bool BarcodeChecker::isValid(const String &barcodeText, BarcodeType type, bool padWithLeadingZeros) {
  return isValid(barcodeText.c_str(), type, padWithLeadingZeros);
}

bool BarcodeChecker::isValid(uint64_t codeNumber, BarcodeType type, bool padWithLeadingZeros) {
  char barcodeText[MAX_BARCODE_LENGTH + 1];
  BarcodeHelper::uint64ToStr(codeNumber, barcodeText);
  return isValid(barcodeText, type, padWithLeadingZeros);
}

bool BarcodeChecker::isValid(const char *barcodeText, BarcodeType type, bool _padWithLeadingZeros) {
  if (type == BarcodeType::Unknown) {
    return detectType(barcodeText, _padWithLeadingZeros) != BarcodeType::Unknown;
  }
  else {
    if (_padWithLeadingZeros) {
      char newBarcodeText[MAX_BARCODE_LENGTH + 1];
      padWithLeadingZeros(newBarcodeText, barcodeText, type);
      barcodeText = newBarcodeText;
    }

    BarcodeType checkedType = detectType(barcodeText, false);
    if (type == checkedType) {
      return true;
    }
    else if (type == BarcodeType::EAN8 && checkedType == BarcodeType::UPCE) {
      return true;
    }
    else {
      return false;
    }
  }
}

/////////////////////////////////////////////////
// detectType methods
/////////////////////////////////////////////////

BarcodeType BarcodeChecker::detectType(const String &barcodeText, bool padWithLeadingZeros){
  return detectType(barcodeText.c_str(), padWithLeadingZeros);
}

BarcodeType BarcodeChecker::detectType(uint64_t codeNumber, bool padWithLeadingZeros){
  char barcodeText[MAX_BARCODE_LENGTH + 1];
  BarcodeHelper::uint64ToStr(codeNumber, barcodeText);
  return detectType(barcodeText, padWithLeadingZeros);
}

BarcodeType BarcodeChecker::detectType(const char *barcodeText, bool _padWithLeadingZeros) {
  // check if every character is a digit
  int textLength = strnlen(barcodeText, 20);
  for (int i = 0; i < textLength; i++) {
    int digit = barcodeText[i] - '0';
    if (digit < 0 || digit > 9) {
      return BarcodeType::Unknown;
    }
  }

  // check text size
  BarcodeType possibleType;
  if (textLength == 13) {
    possibleType = BarcodeType::EAN13;
  }
  else if (textLength == 12) {
    possibleType =  BarcodeType::UPCA;
  }
  else if (textLength == 8) {
    if (barcodeText[0] == '0' || barcodeText[0] == '1') {
      possibleType = BarcodeType::UPCE; // might be EAN-8 as well
    }
    else {
      possibleType = BarcodeType::EAN8;
    }
  }
  else if (_padWithLeadingZeros) {
    char newBarcodeText[MAX_BARCODE_LENGTH + 1];
    padWithLeadingZeros(newBarcodeText, barcodeText, BarcodeType::Unknown);
    barcodeText = newBarcodeText;

    return detectType(barcodeText, false);
  }
  else{
    return BarcodeType::Unknown;
  }

  if (validateChecksum(barcodeText, possibleType)) {
    return possibleType;
  }
  else {
    return BarcodeType::Unknown;
  }
}

/////////////////////////////////////////////////
// getNumberOfDigits method
/////////////////////////////////////////////////

uint16_t BarcodeChecker::getNumberOfDigits(BarcodeType type) {
  switch(type){
    case BarcodeType::EAN13:
      return 13;
    case BarcodeType::EAN8:
      return 8;
    case BarcodeType::UPCA:
      return 12;
    case BarcodeType::UPCE:
      return 8;
    case BarcodeType::Unknown:
      return 0;
  }
}

/////////////////////////////////////////////////
// padWithLeadingZeros methods
/////////////////////////////////////////////////

String BarcodeChecker::padWithLeadingZeros(const String &barcodeText, BarcodeType type) {
  char newBarcodeText[MAX_BARCODE_LENGTH + 1];
  padWithLeadingZeros(newBarcodeText, barcodeText.c_str(), type);
  return String(newBarcodeText);
}

String BarcodeChecker::padWithLeadingZeros(uint64_t barcodeNumber, BarcodeType type) {
  char newBarcodeText[MAX_BARCODE_LENGTH + 1];
  padWithLeadingZeros(newBarcodeText, barcodeNumber, type);
  return String(newBarcodeText);
}

void BarcodeChecker::padWithLeadingZeros(char *newBarcodeText, uint64_t barcodeNumber, BarcodeType type) {
  char barcodeText[MAX_BARCODE_LENGTH + 1];
  BarcodeHelper::uint64ToStr(barcodeNumber, barcodeText);
  padWithLeadingZeros(newBarcodeText, barcodeText, type);
}

void BarcodeChecker::padWithLeadingZeros(char *newBarcodeText, const char *barcodeText, BarcodeType type) {
  if (type != BarcodeType::Unknown) {
    int numberOfDigitsForType = getNumberOfDigits(type);
    padWithLeadingZeros(newBarcodeText, barcodeText, numberOfDigitsForType);
  }
  else  {
    int textSize = strnlen(barcodeText, MAX_BARCODE_LENGTH);
    if (textSize <= 8) {
      padWithLeadingZeros(newBarcodeText, barcodeText, 8);
    }
    else {
      padWithLeadingZeros(newBarcodeText, barcodeText, 13);
    }
  }
}

void BarcodeChecker::padWithLeadingZeros(char *newBarcodeText, const char *barcodeText, int finalLength) {
  int textSize = strnlen(barcodeText, MAX_BARCODE_LENGTH);
  int numberOfZeros = finalLength - textSize;
  if (numberOfZeros <= 0) {
    strncpy(newBarcodeText, barcodeText, textSize+1);
    return;
  }

  uint8_t i = 0;
  while(i<numberOfZeros) {
    newBarcodeText[i] = '0';
    i++;
  }
  strncpy(newBarcodeText + i, barcodeText, textSize+1);
}

/////////////////////////////////////////////////
// validateChecksum method
/////////////////////////////////////////////////

bool BarcodeChecker::validateChecksum(const char *barcodeText, BarcodeType type) {
  int sum = 0;
  int textLength = strnlen(barcodeText, 20);
  int remainderForMultiplication = 0;
  if (type == BarcodeType::EAN13) {
    remainderForMultiplication = 1;
  }
  for (int i = 0; i < textLength - 1; i++) {
    int digit = barcodeText[i] - '0';
    if (i % 2 == remainderForMultiplication) {
      sum += 3 * digit;
    } else {
      sum += digit;
    }
  }
  
  int checksum = (10 - (sum % 10)) % 10;
  int lastDigit = barcodeText[textLength-1] - '0';

  return checksum == lastDigit;
}

/////////////////////////////////////////////////
// uint64ToStr method
/////////////////////////////////////////////////

void BarcodeHelper::uint64ToStr(uint64_t number, char *text) {
  char temp[MAX_BARCODE_LENGTH + 1];
  int i = 0;

  do {
      temp[i++] = (number % 10) + '0';
      number /= 10;
  } while (number > 0);

  temp[i] = '\0';

  // Invert the string
  int j = 0;
  while (i > 0) {
    text[j++] = temp[--i];
  }
  text[j] = '\0';
}