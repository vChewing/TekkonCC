// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#import "Tekkon.hh"

using namespace Tekkon;

@interface TekkonCCTests_Pinyin : XCTestCase

@end

@implementation TekkonCCTests_Pinyin

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

// =========== PINYIN TRIE TESTS ===========

// Test PinyinTrie basic functionality
- (void)test_Pinyin_PinyinTrieBasicOperations {
  PinyinTrie trie(ofHanyuPinyin);

  // Test search for a simple pinyin
  auto results = trie.search("shi");
  XCTAssertFalse(results.empty());
  XCTAssertTrue(std::find(results.begin(), results.end(), "ㄕ") !=
                results.end());

  // Test search for pinyin with multiple results
  results = trie.search("zh");
  XCTAssertFalse(results.empty());
  XCTAssertTrue(std::find(results.begin(), results.end(), "ㄓ") !=
                results.end());
}

// Test PinyinTrie chop functionality
- (void)test_Pinyin_PinyinTrieChopFunction {
  PinyinTrie trie(ofHanyuPinyin);

  // Test chopping a simple pinyin sequence
  // "shjdaz" should be chopped to something like ["sh", "j", "da", "z"] or
  // similar
  auto chopped = trie.chop("shjdaz");
  XCTAssertFalse(chopped.empty());
  XCTAssertGreaterThan(chopped.size(),
                       1UL);  // Should be split into multiple parts

  // Test chopping a more complex sequence
  chopped = trie.chop("zhongguo");
  XCTAssertFalse(chopped.empty());
  // Should contain "zhong" or parts of it, and "guo" or parts of it
}

// Test deductChoppedPinyinToZhuyin functionality
- (void)test_Pinyin_DeductChoppedPinyinToZhuyin {
  PinyinTrie trie(ofHanyuPinyin);

  // Test with a simple sequence
  std::vector<std::string> chopped = {"b", "yue", "z", "q", "s", "l", "l"};
  auto zhuyinCandidates = trie.deductChoppedPinyinToZhuyin(chopped);

  XCTAssertEqual(zhuyinCandidates.size(), chopped.size());

  // "b" should map to "ㄅ"
  XCTAssertTrue(zhuyinCandidates[0].find("ㄅ") != std::string::npos);

  // "yue" should map to "ㄩㄝ"
  XCTAssertTrue(zhuyinCandidates[1].find("ㄩㄝ") != std::string::npos);

  // "q" should map to "ㄑ"
  XCTAssertTrue(zhuyinCandidates[3].find("ㄑ") != std::string::npos);

  // "l" should map to "ㄌ"
  XCTAssertTrue(zhuyinCandidates[5].find("ㄌ") != std::string::npos);
}

// Test PinyinTrie with different parsers
- (void)test_Pinyin_PinyinTrieDifferentParsers {
  // Test with Secondary Pinyin
  PinyinTrie trieSecondary(ofSecondaryPinyin);
  auto results = trieSecondary.search("jr");
  XCTAssertFalse(results.empty());

  // Test with Yale Pinyin
  PinyinTrie trieYale(ofYalePinyin);
  results = trieYale.search("jr");
  XCTAssertFalse(results.empty());
}

// Test integrated chop and deduct workflow
- (void)test_Pinyin_IntegratedChopAndDeduct {
  PinyinTrie trie(ofHanyuPinyin);

  // Simulate the complete workflow: input -> chop -> deduct -> zhuyin
  std::string pinyinInput = "shijie";

  // Step 1: Chop the pinyin
  auto chopped = trie.chop(pinyinInput);
  XCTAssertFalse(chopped.empty());

  // Step 2: Deduct to zhuyin
  auto zhuyinCandidates = trie.deductChoppedPinyinToZhuyin(chopped);
  XCTAssertEqual(zhuyinCandidates.size(), chopped.size());

  // Verify we got some zhuyin output
  bool hasZhuyin = false;
  for (const auto& candidate : zhuyinCandidates) {
    if (!candidate.empty() && candidate.find("ㄕ") != std::string::npos) {
      hasZhuyin = true;
      break;
    }
  }
  XCTAssertTrue(hasZhuyin);
}

@end
