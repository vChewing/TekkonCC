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

@interface TekkonCCTests_Intermediate : XCTestCase

@end

@implementation TekkonCCTests_Intermediate

// =========== PINYIN TYPINNG HANDLING TESTS ===========

- (void)test_Pinyin_HanyuinyinKeyReceivingAndCompositions {
  Composer composer = Composer("", ofHanyuPinyin);
  bool toneMarkerIndicator;

  // Test Key Receiving
  composer.receiveKey(100);  // d
  composer.receiveKey("i");
  composer.receiveKey("a");
  composer.receiveKey("o");
  XCTAssertEqual(composer.consonant.value(), "ㄉ");
  XCTAssertEqual(composer.semivowel.value(), "ㄧ");
  XCTAssertEqual(composer.vowel.value(), "ㄠ");

  // Testing missing tone markers;
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("3");  // 上聲
  XCTAssertEqual(composer.value(), "ㄉㄧㄠˇ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄉㄧㄠ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition;
  XCTAssertEqual(composer.getComposition(), "ㄉㄧㄠ");
  XCTAssertEqual(composer.getComposition(true, false), "diao1");
  XCTAssertEqual(composer.getComposition(true, true), "diāo");
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true), "diao1");

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄉㄧㄠ˙");
  XCTAssertEqual(composer.getComposition(false, true), "˙ㄉㄧㄠ");

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
}

- (void)test_Pinyin_SecondaryPinyinKeyReceivingAndCompositions {
  Composer composer = Composer("", ofSecondaryPinyin);
  bool toneMarkerIndicator;

  // Test Key Receiving
  composer.receiveKey(99);  // c
  composer.receiveKey("h");
  composer.receiveKey("i");
  composer.receiveKey("u");
  composer.receiveKey("n");
  composer.receiveKey("g");
  XCTAssertEqual(composer.consonant.value(), "ㄑ");
  XCTAssertEqual(composer.semivowel.value(), "ㄩ");
  XCTAssertEqual(composer.vowel.value(), "ㄥ");

  // Testing missing tone markers
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("2");  // 陽平
  XCTAssertEqual(composer.value(), "ㄑㄩㄥˊ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄑㄩㄥ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition;
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ");
  XCTAssertEqual(composer.getComposition(true, false), "qiong1");
  XCTAssertEqual(composer.getComposition(true, true), "qiōng");
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true), "chiung1");

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ˙");
  XCTAssertEqual(composer.getComposition(false, true), "˙ㄑㄩㄥ");

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
}

- (void)test_Pinyin_YalePinyinKeyReceivingAndCompositions {
  Composer composer = Composer("", ofYalePinyin);
  bool toneMarkerIndicator;

  // Test Key Receiving
  composer.receiveKey(99);  // c
  composer.receiveKey("h");
  composer.receiveKey("y");
  composer.receiveKey("u");
  composer.receiveKey("n");
  composer.receiveKey("g");
  XCTAssertEqual(composer.consonant.value(), "ㄑ");
  XCTAssertEqual(composer.semivowel.value(), "ㄩ");
  XCTAssertEqual(composer.vowel.value(), "ㄥ");

  // Testing missing tone markers
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("2");  // 陽平
  XCTAssertEqual(composer.value(), "ㄑㄩㄥˊ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄑㄩㄥ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition;
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ");
  XCTAssertEqual(composer.getComposition(true, false), "qiong1");
  XCTAssertEqual(composer.getComposition(true, true), "qiōng");
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true), "chyung1");

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ˙");
  XCTAssertEqual(composer.getComposition(false, true), "˙ㄑㄩㄥ");

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
}

- (void)test_Pinyin_HualuoPinyinKeyReceivingAndCompositions {
  Composer composer = Composer("", ofHualuoPinyin);
  bool toneMarkerIndicator;

  // Test Key Receiving
  composer.receiveKey(99);  // c
  composer.receiveKey("h");
  composer.receiveKey("y");
  composer.receiveKey("o");
  composer.receiveKey("n");
  composer.receiveKey("g");
  XCTAssertEqual(composer.consonant.value(), "ㄑ");
  XCTAssertEqual(composer.semivowel.value(), "ㄩ");
  XCTAssertEqual(composer.vowel.value(), "ㄥ");

  // Testing missing tone markers
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("2");  // 陽平
  XCTAssertEqual(composer.value(), "ㄑㄩㄥˊ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄑㄩㄥ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ");
  XCTAssertEqual(composer.getComposition(true, false), "qiong1");
  XCTAssertEqual(composer.getComposition(true, true), "qiōng");
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true), "chyong1");

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ˙");
  XCTAssertEqual(composer.getComposition(false, true), "˙ㄑㄩㄥ");

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
}

- (void)test_Pinyin_UniversalPinyinKeyReceivingAndCompositions {
  Composer composer = Composer("", ofUniversalPinyin);
  bool toneMarkerIndicator;

  // Test Key Receiving
  composer.receiveKey(99);  // c
  composer.receiveKey("y");
  composer.receiveKey("o");
  composer.receiveKey("n");
  composer.receiveKey("g");
  XCTAssertEqual(composer.consonant.value(), "ㄑ");
  XCTAssertEqual(composer.semivowel.value(), "ㄩ");
  XCTAssertEqual(composer.vowel.value(), "ㄥ");

  // Testing missing tone markers
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("2");  // 陽平
  XCTAssertEqual(composer.value(), "ㄑㄩㄥˊ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄑㄩㄥ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ");
  XCTAssertEqual(composer.getComposition(true, false), "qiong1");
  XCTAssertEqual(composer.getComposition(true, true), "qiōng");
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true), "cyong1");

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ˙");
  XCTAssertEqual(composer.getComposition(false, true), "˙ㄑㄩㄥ");

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
}

- (void)test_Pinyin_WadeGilesPinyinKeyReceivingAndCompositions {
  Composer composer = Composer("", ofWadeGilesPinyin);
  bool toneMarkerIndicator;

  // Test Key Receiving
  composer.receiveKey(99);  // c
  composer.receiveKey("h");
  composer.receiveKey("'");  // 韋氏拼音清濁分辨鍵
  composer.receiveKey("i");
  composer.receiveKey("u");
  composer.receiveKey("n");
  composer.receiveKey("g");

  // Testing missing tone markers
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("2");  // 陽平
  XCTAssertEqual(composer.value(), "ㄑㄩㄥˊ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄑㄩㄥ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ");
  XCTAssertEqual(composer.getComposition(true, false), "qiong1");
  XCTAssertEqual(composer.getComposition(true, true), "qiōng");
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true), "ch'iung1");

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄑㄩㄥ˙");
  XCTAssertEqual(composer.getComposition(false, true), "˙ㄑㄩㄥ");

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
}

@end
