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

}  // namespace Tekkon
