// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

#include <string>
#include <vector>

#include "../Sources/Tekkon/include/Tekkon.hh"
#include "gtest/gtest.h"

namespace Tekkon {

// Test PinyinTrie basic functionality
TEST(TekkonTests_Pinyin, PinyinTrieBasicOperations) {
  PinyinTrie trie(ofHanyuPinyin);

  // Test search for a simple pinyin
  auto results = trie.search("shi");
  ASSERT_FALSE(results.empty());
  ASSERT_TRUE(std::find(results.begin(), results.end(), "ㄕ") != results.end());

  // Test search for pinyin with multiple results
  results = trie.search("zh");
  ASSERT_FALSE(results.empty());
  ASSERT_TRUE(std::find(results.begin(), results.end(), "ㄓ") != results.end());
}

// Test PinyinTrie chop functionality
TEST(TekkonTests_Pinyin, PinyinTrieChopFunction) {
  PinyinTrie trie(ofHanyuPinyin);

  // Test chopping a simple pinyin sequence
  // "shjdaz" should be chopped to something like ["sh", "j", "da", "z"] or
  // similar
  auto chopped = trie.chop("shjdaz");
  ASSERT_FALSE(chopped.empty());
  ASSERT_GT(chopped.size(), 1);  // Should be split into multiple parts

  // Test chopping a more complex sequence
  chopped = trie.chop("zhongguo");
  ASSERT_FALSE(chopped.empty());
  // Should contain "zhong" or parts of it, and "guo" or parts of it
}

// Test deductChoppedPinyinToZhuyin functionality
TEST(TekkonTests_Pinyin, DeductChoppedPinyinToZhuyin) {
  PinyinTrie trie(ofHanyuPinyin);

  // Test with a simple sequence
  std::vector<std::string> chopped = {"b", "yue", "z", "q", "s", "l", "l"};
  auto zhuyinCandidates = trie.deductChoppedPinyinToZhuyin(chopped);

  ASSERT_EQ(zhuyinCandidates.size(), chopped.size());

  // "b" should map to "ㄅ"
  ASSERT_TRUE(zhuyinCandidates[0].find("ㄅ") != std::string::npos);

  // "yue" should map to "ㄩㄝ"
  ASSERT_TRUE(zhuyinCandidates[1].find("ㄩㄝ") != std::string::npos);

  // "q" should map to "ㄑ"
  ASSERT_TRUE(zhuyinCandidates[3].find("ㄑ") != std::string::npos);

  // "l" should map to "ㄌ"
  ASSERT_TRUE(zhuyinCandidates[5].find("ㄌ") != std::string::npos);
}

// Test PinyinTrie with different parsers
TEST(TekkonTests_Pinyin, PinyinTrieDifferentParsers) {
  // Test with Secondary Pinyin
  PinyinTrie trieSecondary(ofSecondaryPinyin);
  auto results = trieSecondary.search("jr");
  ASSERT_FALSE(results.empty());

  // Test with Yale Pinyin
  PinyinTrie trieYale(ofYalePinyin);
  results = trieYale.search("jr");
  ASSERT_FALSE(results.empty());
}

// Test integrated chop and deduct workflow
TEST(TekkonTests_Pinyin, IntegratedChopAndDeduct) {
  PinyinTrie trie(ofHanyuPinyin);

  // Simulate the complete workflow: input -> chop -> deduct -> zhuyin
  std::string pinyinInput = "shijie";

  // Step 1: Chop the pinyin
  auto chopped = trie.chop(pinyinInput);
  ASSERT_FALSE(chopped.empty());

  // Step 2: Deduct to zhuyin
  auto zhuyinCandidates = trie.deductChoppedPinyinToZhuyin(chopped);
  ASSERT_EQ(zhuyinCandidates.size(), chopped.size());

  // Verify we got some zhuyin output
  bool hasZhuyin = false;
  for (const auto& candidate : zhuyinCandidates) {
    if (!candidate.empty() && candidate.find("ㄕ") != std::string::npos) {
      hasZhuyin = true;
      break;
    }
  }
  ASSERT_TRUE(hasZhuyin);
}

// Test PinyinAutoChopResult functionality
TEST(TekkonTests_Pinyin, PinyinAutoChopResult) {
  Composer composer("", ofHanyuPinyin);
  composer.receiveKey("s");
  composer.receiveKey("h");
  composer.receiveKey("i");

  auto autoChop = composer.pinyinAutoChopResult("j");
  ASSERT_TRUE(autoChop.has_value());
  ASSERT_EQ(autoChop.value().committedReadings.size(), 1);
  ASSERT_EQ(autoChop.value().committedReadings[0], "ㄕ");
  ASSERT_EQ(autoChop.value().remainingRomaji, "j");

  composer.replacePinyinBuffer(autoChop.value().remainingRomaji);
  ASSERT_EQ(composer.getInlineCompositionForDisplay(true), "j");
  ASSERT_FALSE(composer.isPronounceable());
}

// Test zhuyinReadings: exact complete syllable
TEST(TekkonTests_Pinyin, PinyinTrieZhuyinReadingsExactCompleteSyllable) {
  // 漢語拼音：
  PinyinTrie trie(ofHanyuPinyin);
  auto shi = trie.zhuyinReadings("shi");
  ASSERT_EQ(shi.size(), 1);
  ASSERT_EQ(shi[0], "ㄕ");

  auto ni = trie.zhuyinReadings("ni");
  ASSERT_EQ(ni.size(), 1);
  ASSERT_EQ(ni[0], "ㄋㄧ");

  // "nan" 同時是 "nang" 的字串前綴；精確匹配時不展開後者。
  auto nan = trie.zhuyinReadings("nan");
  ASSERT_EQ(nan.size(), 1);
  ASSERT_EQ(nan[0], "ㄋㄢ");

  // 國音二式：
  PinyinTrie trieSecondary(ofSecondaryPinyin);
  auto chiung = trieSecondary.zhuyinReadings("chiung");
  ASSERT_EQ(chiung.size(), 1);
  ASSERT_EQ(chiung[0], "ㄑㄩㄥ");
}

// Test zhuyinReadings: incomplete prefix expansion
TEST(TekkonTests_Pinyin, PinyinTrieZhuyinReadingsIncompletePrefixExpansion) {
  PinyinTrie trie(ofHanyuPinyin);

  // "z" 同時是 z- 與 zh- 兩系音節的字串前綴：兩種聲母的注音都應涵蓋。
  auto zReadings = trie.zhuyinReadings("z");
  ASSERT_FALSE(zReadings.empty());
  auto zDedup = zReadings;
  std::sort(zDedup.begin(), zDedup.end());
  zDedup.erase(std::unique(zDedup.begin(), zDedup.end()), zDedup.end());
  ASSERT_EQ(zDedup.size(), zReadings.size());  // 去重。
  ASSERT_TRUE(std::is_sorted(zReadings.begin(),
                             zReadings.end()));  // 排序穩定（Unicode 字典序）。
  ASSERT_TRUE(std::find(zReadings.begin(), zReadings.end(), "ㄗ") !=
              zReadings.end());
  ASSERT_TRUE(std::find(zReadings.begin(), zReadings.end(), "ㄓ") !=
              zReadings.end());

  // "zh" 前綴只涵蓋 zh- 系。
  auto zhReadings = trie.zhuyinReadings("zh");
  ASSERT_FALSE(zhReadings.empty());
  ASSERT_TRUE(std::is_sorted(zhReadings.begin(), zhReadings.end()));
  ASSERT_TRUE(std::find(zhReadings.begin(), zhReadings.end(), "ㄓ") !=
              zhReadings.end());
  ASSERT_TRUE(std::find(zhReadings.begin(), zhReadings.end(), "ㄗ") ==
              zhReadings.end());
  // 確定性：重複呼叫輸出一致。
  ASSERT_TRUE(zhReadings == trie.zhuyinReadings("zh"));
}

// Test zhuyinReadings: edge cases
TEST(TekkonTests_Pinyin, PinyinTrieZhuyinReadingsEdgeCases) {
  // 空字串：回傳空陣列。
  PinyinTrie trie(ofHanyuPinyin);
  ASSERT_TRUE(trie.zhuyinReadings("").empty());
  // 非拼音排列（大千注音）：直接回傳空陣列。
  PinyinTrie trieDachen(ofDachen);
  ASSERT_TRUE(trieDachen.zhuyinReadings("z").empty());
  // 不可能的前綴：無任何音節以之開頭。
  ASSERT_TRUE(trie.zhuyinReadings("xw").empty());
}

// Test BackSpace resyncs phonabet slots in pinyin mode
TEST(TekkonTests_Pinyin, BackSpaceResyncsPhonabetSlots) {
  Composer composer("", ofHanyuPinyin);

  // 輸入完整音節「ma」。
  composer.receiveKey("m");
  composer.receiveKey("a");
  ASSERT_EQ(composer.romajiBuffer, "ma");
  ASSERT_TRUE(composer.isPronounceable());

  // 兩次 BackSpace 清空緩衝：聲介韻槽位須同步清空。
  composer.doBackSpace();
  ASSERT_EQ(composer.romajiBuffer, "m");
  composer.doBackSpace();
  ASSERT_TRUE(composer.romajiBuffer.empty());
  ASSERT_TRUE(composer.isEmpty());
  ASSERT_FALSE(composer.isPronounceable());

  // 清空後收下陰平空格鍵：不應把已刪除的「ma」重新組回。
  composer.receiveKey(" ");  // 陰平
  ASSERT_EQ(composer.intonation.value(), " ");
  ASSERT_EQ(composer.getComposition(), "");
  ASSERT_FALSE(composer.isPronounceable());
}

// Test extended romaji buffer preserves long abbreviation stream
TEST(TekkonTests_Pinyin, ExtendedRomajiBufferPreservesLongAbbreviationStream) {
  // 預設（false）：超過 6 碼即丟棄最早音頭——「slliang」第 7 碼「g」觸發、
  // buffer 變「lliang」。
  Composer capped("", ofHanyuPinyin);
  for (char ch : std::string("slliang")) capped.receiveKey(std::string(1, ch));
  ASSERT_EQ(capped.romajiBuffer, "lliang");

  // 啟用（true）：完整保留多音節簡拼字母流。
  Composer extended("", ofHanyuPinyin);
  extended.allowsExtendedRomajiBuffer = true;
  for (char ch : std::string("slliang"))
    extended.receiveKey(std::string(1, ch));
  ASSERT_EQ(extended.romajiBuffer, "slliang");
}

}  // namespace Tekkon
