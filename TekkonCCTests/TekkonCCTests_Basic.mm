// (c) 2022 and onwards The vChewing Project (MIT-NTL License).
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

2. No trademark license is granted to use the trade names, trademarks, service
marks, or product names of Contributor, except as required to fulfill notice
requirements above.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// ADVICE: Save as UTF8 without BOM signature!!!

#import <XCTest/XCTest.h>

#import "Tekkon.hh"

using namespace Tekkon;

@interface TekkonCCTests_Basic : XCTestCase

@end

@implementation TekkonCCTests_Basic

- (void)test_Basic_InitializingPhonabet {
  Phonabet thePhonabetNull = Phonabet("0");
  Phonabet thePhonabetA = Phonabet("ㄉ");
  Phonabet thePhonabetB = Phonabet("ㄧ");
  Phonabet thePhonabetC = Phonabet("ㄠ");
  Phonabet thePhonabetD = Phonabet("ˇ");
  XCTAssertEqual(thePhonabetNull.type, PhoneType::null);
  XCTAssertEqual(thePhonabetA.type, PhoneType::consonant);
  XCTAssertEqual(thePhonabetB.type, PhoneType::semivowel);
  XCTAssertEqual(thePhonabetC.type, PhoneType::vowel);
  XCTAssertEqual(thePhonabetD.type, PhoneType::intonation);
}

- (void)test_Basic_IsValidKeyWithKeys {
  bool result;
  Tekkon::Composer composer = Composer("", ofDachen);

  /// Testing Failed Key
  result = composer.inputValidityCheck(0x0024);
  XCTAssertFalse(result);

  // Testing Correct Qwerty Dachen Key
  composer.ensureParser(ofDachen);
  result = composer.inputValidityCheck(0x002F);
  XCTAssertTrue(result);

  // Testing Correct Eten26 Key
  composer.ensureParser(ofEten26);
  result = composer.inputValidityCheck(0x0062);
  XCTAssertTrue(result);

  // Testing Correct Hanyu-Pinyin Key
  composer.ensureParser(ofHanyuPinyin);
  result = composer.inputValidityCheck(0x0062);
  XCTAssertTrue(result);
}

// =========== PHONABET TYPINNG HANDLING TESTS (BASIC) ===========

- (void)test_Basic_PhonabetKeyReceivingAndCompositions {
  Composer composer = Composer("", ofDachen);
  bool toneMarkerIndicator;

  // Test Key Receiving;
  composer.receiveKey(0x0032);  // 2, ㄉ
  composer.receiveKey("j");     // ㄨ
  composer.receiveKey("u");     // ㄧ
  composer.receiveKey("l");     // ㄠ

  // Testing missing tone markers;
  toneMarkerIndicator = composer.hasToneMarker();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("3");  // 上聲
  XCTAssertEqual(composer.value(), "ㄉㄧㄠˇ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄉㄧㄠ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition
  XCTAssertEqual(composer.getComposition(), "ㄉㄧㄠ");
  XCTAssertEqual(composer.getComposition(true, false),
                 "diao1");  // 中階測試項目
  XCTAssertEqual(composer.getComposition(true, true),
                 "diāo");  // 中階測試項目
  XCTAssertEqual(composer.getInlineCompositionForIMK(true),
                 "diao1");  // 中階測試項目

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄉㄧㄠ˙");
  XCTAssertEqual(composer.getComposition(false, true),
                 "˙ㄉㄧㄠ");  // 中階測試項目

  // Testing having tone markers
  toneMarkerIndicator = composer.hasToneMarker();
  XCTAssertTrue(toneMarkerIndicator);

  // Testing having not-only tone markers
  toneMarkerIndicator = composer.hasToneMarker(true);
  XCTAssertTrue(!toneMarkerIndicator);

  // Testing having only tone markers
  composer.clear();
  composer.receiveKey("3");  // 上聲
  toneMarkerIndicator = composer.hasToneMarker(true);
  XCTAssertTrue(toneMarkerIndicator);
}

@end