#include <Arduino.h>


enum class BarcodeType {
  Unknown,
  EAN13,
  EAN8,
  UPCA,
  UPCE
};


class BarcodeChecker {
public:
  // check if codeText is a valid barcode
  // if BarcodeType is Unknown, it will detect the type
  // this method is called before drawing, but you can manually check it too
  static bool isValid(const char *barcodeText,   BarcodeType type = BarcodeType::Unknown, bool padWithLeadingZeros = true);
  static bool isValid(const String &barcodeText, BarcodeType type = BarcodeType::Unknown, bool padWithLeadingZeros = true);
  static bool isValid(uint64_t barcodeNumber,    BarcodeType type = BarcodeType::Unknown, bool padWithLeadingZeros = true);

  static BarcodeType detectType(const char *barcodeText,   bool padWithLeadingZeros = true);
  static BarcodeType detectType(const String &barcodeText, bool padWithLeadingZeros = true);
  static BarcodeType detectType(uint64_t barcodeNumber,    bool padWithLeadingZeros = true);

  static uint16_t getNumberOfDigits(BarcodeType type);

  static void padWithLeadingZeros(char *newBarcodeText, const char *barcodeText, BarcodeType type);
  static void padWithLeadingZeros(char *newBarcodeText, uint64_t barcodeNumber, BarcodeType type);
  static String padWithLeadingZeros(const String &barcodeText, BarcodeType type);
  static String padWithLeadingZeros(uint64_t barcodeNumber, BarcodeType type);


private:
  static void padWithLeadingZeros(char *newBarcodeText, const char *barcodeText, int finalLength);

  static bool validateChecksum(const char *barcodeText, BarcodeType type);
};


class BarcodeHelper {
public:
  static void uint64ToStr(uint64_t number, char *text);
};