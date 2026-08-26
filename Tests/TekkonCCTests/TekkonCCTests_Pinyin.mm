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

- (void)test_Pinyin_PinyinAutoChopResult {
  Composer composer("", ofHanyuPinyin);
  composer.receiveKey("s");
  composer.receiveKey("h");
  composer.receiveKey("i");

  auto autoChop = composer.pinyinAutoChopResult("j");
  XCTAssertTrue(autoChop.has_value());
  XCTAssertEqual(autoChop.value().committedReadings.size(), 1);
  XCTAssertEqual(autoChop.value().committedReadings[0], "ㄕ");
  XCTAssertEqual(autoChop.value().remainingRomaji, "j");

  composer.replacePinyinBuffer(autoChop.value().remainingRomaji);
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true), "j");
  XCTAssertFalse(composer.isPronounceable());
}

// Test zhuyinReadings: exact complete syllable
- (void)test_Pinyin_PinyinTrieZhuyinReadingsExactCompleteSyllable {
  // 漢語拼音：
  PinyinTrie trie(ofHanyuPinyin);
  auto shi = trie.zhuyinReadings("shi");
  XCTAssertEqual(shi.size(), 1UL);
  XCTAssertEqual(shi[0], "ㄕ");

  auto ni = trie.zhuyinReadings("ni");
  XCTAssertEqual(ni.size(), 1UL);
  XCTAssertEqual(ni[0], "ㄋㄧ");

  // "nan" 同時是 "nang" 的字串前綴；精確匹配時不展開後者。
  auto nan = trie.zhuyinReadings("nan");
  XCTAssertEqual(nan.size(), 1UL);
  XCTAssertEqual(nan[0], "ㄋㄢ");

  // 國音二式：
  PinyinTrie trieSecondary(ofSecondaryPinyin);
  auto chiung = trieSecondary.zhuyinReadings("chiung");
  XCTAssertEqual(chiung.size(), 1UL);
  XCTAssertEqual(chiung[0], "ㄑㄩㄥ");
}

// Test zhuyinReadings: incomplete prefix expansion
- (void)test_Pinyin_PinyinTrieZhuyinReadingsIncompletePrefixExpansion {
  PinyinTrie trie(ofHanyuPinyin);

  // "z" 同時是 z- 與 zh- 兩系音節的字串前綴：兩種聲母的注音都應涵蓋。
  auto zReadings = trie.zhuyinReadings("z");
  XCTAssertFalse(zReadings.empty());
  auto zDedup = zReadings;
  std::sort(zDedup.begin(), zDedup.end());
  zDedup.erase(std::unique(zDedup.begin(), zDedup.end()), zDedup.end());
  XCTAssertEqual(zDedup.size(), zReadings.size());  // 去重。
  XCTAssertTrue(
      std::is_sorted(zReadings.begin(),
                     zReadings.end()));  // 排序穩定（Unicode 字典序）。
  XCTAssertTrue(std::find(zReadings.begin(), zReadings.end(), "ㄗ") !=
                zReadings.end());
  XCTAssertTrue(std::find(zReadings.begin(), zReadings.end(), "ㄓ") !=
                zReadings.end());

  // "zh" 前綴只涵蓋 zh- 系。
  auto zhReadings = trie.zhuyinReadings("zh");
  XCTAssertFalse(zhReadings.empty());
  XCTAssertTrue(std::is_sorted(zhReadings.begin(), zhReadings.end()));
  XCTAssertTrue(std::find(zhReadings.begin(), zhReadings.end(), "ㄓ") !=
                zhReadings.end());
  XCTAssertTrue(std::find(zhReadings.begin(), zhReadings.end(), "ㄗ") ==
                zhReadings.end());
  // 確定性：重複呼叫輸出一致。
  XCTAssertTrue(zhReadings == trie.zhuyinReadings("zh"));
}

// Test zhuyinReadings: edge cases
- (void)test_Pinyin_PinyinTrieZhuyinReadingsEdgeCases {
  // 空字串：回傳空陣列。
  PinyinTrie trie(ofHanyuPinyin);
  XCTAssertTrue(trie.zhuyinReadings("").empty());
  // 非拼音排列（大千注音）：直接回傳空陣列。
  PinyinTrie trieDachen(ofDachen);
  XCTAssertTrue(trieDachen.zhuyinReadings("z").empty());
  // 不可能的前綴：無任何音節以之開頭。
  XCTAssertTrue(trie.zhuyinReadings("xw").empty());
}

// Test BackSpace resyncs phonabet slots in pinyin mode
- (void)test_Pinyin_BackSpaceResyncsPhonabetSlots {
  Composer composer("", ofHanyuPinyin);

  // 輸入完整音節「ma」。
  composer.receiveKey("m");
  composer.receiveKey("a");
  XCTAssertEqual(composer.romajiBuffer, "ma");
  XCTAssertTrue(composer.isPronounceable());

  // 兩次 BackSpace 清空緩衝：聲介韻槽位須同步清空。
  composer.doBackSpace();
  XCTAssertEqual(composer.romajiBuffer, "m");
  composer.doBackSpace();
  XCTAssertTrue(composer.romajiBuffer.empty());
  XCTAssertTrue(composer.isEmpty());
  XCTAssertFalse(composer.isPronounceable());

  // 清空後收下陰平空格鍵：不應把已刪除的「ma」重新組回。
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.intonation.value(), " ");
  XCTAssertEqual(composer.getComposition(), "");
  XCTAssertFalse(composer.isPronounceable());
}

@end
