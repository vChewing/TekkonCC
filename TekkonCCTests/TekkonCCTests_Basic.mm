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

  // Testing Correct ETen26 Key
  composer.ensureParser(ofETen26);
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
  toneMarkerIndicator = composer.hasIntonation();
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
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true),
                 "diao1");  // 中階測試項目

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄉㄧㄠ˙");
  XCTAssertEqual(composer.getComposition(false, true),
                 "˙ㄉㄧㄠ");  // 中階測試項目

  // Testing having tone markers
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(toneMarkerIndicator);

  // Testing having not-only tone markers
  toneMarkerIndicator = composer.hasIntonation(true);
  XCTAssertTrue(!toneMarkerIndicator);

  // Testing having only tone markers
  composer.clear();
  composer.receiveKey("3");  // 上聲
  toneMarkerIndicator = composer.hasIntonation(true);
  XCTAssertTrue(toneMarkerIndicator);

  // Testing auto phonabet combination fixing process.
  composer.phonabetCombinationCorrectionEnabled = true;

  // Testing exceptions of handling "ㄅㄨㄛ ㄆㄨㄛ ㄇㄨㄛ ㄈㄨㄛ"
  composer.clear();
  composer.receiveKey("1");
  composer.receiveKey("j");
  composer.receiveKey("i");
  XCTAssertEqual(composer.getComposition(), "ㄅㄛ");
  composer.receiveKey("q");
  XCTAssertEqual(composer.getComposition(), "ㄆㄛ");
  composer.receiveKey("a");
  XCTAssertEqual(composer.getComposition(), "ㄇㄛ");
  composer.receiveKey("z");
  XCTAssertEqual(composer.getComposition(), "ㄈㄛ");

  // Testing exceptions of handling "ㄅㄨㄥ ㄆㄨㄥ ㄇㄨㄥ ㄈㄨㄥ"
  composer.clear();
  composer.receiveKey("1");
  composer.receiveKey("j");
  composer.receiveKey("/");
  XCTAssertEqual(composer.getComposition(), "ㄅㄥ");
  composer.receiveKey("q");
  XCTAssertEqual(composer.getComposition(), "ㄆㄥ");
  composer.receiveKey("a");
  XCTAssertEqual(composer.getComposition(), "ㄇㄥ");
  composer.receiveKey("z");
  XCTAssertEqual(composer.getComposition(), "ㄈㄥ");

  // Testing exceptions of handling "ㄋㄨㄟ ㄌㄨㄟ"
  composer.clear();
  composer.receiveKey("s");
  composer.receiveKey("j");
  composer.receiveKey("o");
  XCTAssertEqual(composer.getComposition(), "ㄋㄟ");
  composer.receiveKey("x");
  XCTAssertEqual(composer.getComposition(), "ㄌㄟ");

  // Testing exceptions of handling "ㄧㄜ ㄩㄜ"
  composer.clear();
  composer.receiveKey("s");
  composer.receiveKey("k");
  composer.receiveKey("u");
  XCTAssertEqual(composer.getComposition(), "ㄋㄧㄝ");
  composer.receiveKey("s");
  composer.receiveKey("m");
  composer.receiveKey("k");
  XCTAssertEqual(composer.getComposition(), "ㄋㄩㄝ");
  composer.receiveKey("s");
  composer.receiveKey("u");
  composer.receiveKey("k");
  XCTAssertEqual(composer.getComposition(), "ㄋㄧㄝ");

  // Testing exceptions of handling "ㄨㄜ ㄨㄝ"
  composer.clear();
  composer.receiveKey("j");
  composer.receiveKey("k");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");
  composer.clear();
  composer.receiveKey("j");
  composer.receiveKey(",");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");
  composer.clear();
  composer.receiveKey(",");
  composer.receiveKey("j");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");
  composer.clear();
  composer.receiveKey("k");
  composer.receiveKey("j");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");

  // Testing tool functions
  XCTAssertEqual(Tekkon::restoreToneOneInPhona("ㄉㄧㄠ"), "ㄉㄧㄠ1");
  XCTAssertEqual(Tekkon::cnvPhonaToTextbookReading("ㄓㄜ˙"), "˙ㄓㄜ");
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToPhona("bian4-le5-tian1"),
                 "ㄅㄧㄢˋ-ㄌㄜ˙-ㄊㄧㄢ");
}

@end
