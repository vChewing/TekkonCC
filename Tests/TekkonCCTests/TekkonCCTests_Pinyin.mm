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
