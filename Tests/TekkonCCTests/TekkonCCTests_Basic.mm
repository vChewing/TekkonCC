// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

#import <Foundation/Foundation.h>
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

// =========== COMPOSER POKAYOKE TESTS ===========

- (void)test_Basic_PhonabetCombinationCorrection {
  Composer composer = Composer("", ofDachen, true);

  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄧ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄓˋ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄐㄩˋ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄧ");
  composer.receiveKeyFromPhonabet("ㄢ");
  XCTAssertEqual(composer.value(), "ㄓㄢ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ㄢ");
  XCTAssertEqual(composer.value(), "ㄐㄩㄢ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄧ");
  composer.receiveKeyFromPhonabet("ㄢ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄓㄢˋ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ㄢ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄐㄩㄢˋ");
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
  XCTAssertEqual(Tekkon::cnvPhonaToTextbookStyle("ㄓㄜ˙"), "˙ㄓㄜ");
  XCTAssertEqual(Tekkon::cnvPhonaToHanyuPinyin("ㄍㄢˋ"), "gan4");
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToTextBookStyle("起(qi3)居(ju1)"),
                 "起(qǐ)居(jū)");
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToPhona("bian4-le5-tian1"),
                 "ㄅㄧㄢˋ-ㄌㄜ˙-ㄊㄧㄢ");
  // 測試這種情形：「如果傳入的字串不包含任何半形英數內容的話，那麼應該直接將傳入的字串原樣返回」。
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToPhona("ㄅㄧㄢˋ-˙ㄌㄜ-ㄊㄧㄢ"),
                 "ㄅㄧㄢˋ-˙ㄌㄜ-ㄊㄧㄢ");
}

@end
