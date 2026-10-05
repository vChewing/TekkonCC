// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#import "Tekkon.hh"

using namespace Tekkon;

@interface TekkonCCTests_Utilities : XCTestCase

@end

@implementation TekkonCCTests_Utilities

- (void)test_Utilities_RestoreFirstToneEdgeCases {
  // 空字串防呆。
  XCTAssertEqual(restoreFirstToneInPhona(""), "");
  XCTAssertEqual(restoreFirstToneInPhona("ㄉㄧㄠ"), "ㄉㄧㄠ1");
  XCTAssertEqual(restoreFirstToneInPhona("ㄉㄧㄠˋ"), "ㄉㄧㄠˋ");
  XCTAssertEqual(restoreFirstToneInPhona("ㄉㄧㄠ˙"), "ㄉㄧㄠ˙");
}

- (void)test_Utilities_PhonaToPinyinFullTableSweep {
  // 對照表全表掃描：bucket 化之後每筆條目仍須精確命中。
  for (const auto& pair : arrPhonaToHanyuPinyin) {
    if (pair.size() < 2) continue;
    XCTAssertEqual(cnvPhonaToHanyuPinyin(pair[0]), pair[1]);
  }
}

- (void)test_Utilities_PhonaToPinyinLongestMatch {
  // 最長比對優先：三字組合不得被拆成「聲母＋韻母」。
  XCTAssertEqual(cnvPhonaToHanyuPinyin("ㄅㄧㄥ"), "bing");
  XCTAssertEqual(cnvPhonaToHanyuPinyin("ㄅㄧㄥˋ"), "bing4");
  // 未命中條目的字元原樣保留。
  XCTAssertEqual(cnvPhonaToHanyuPinyin("幹"), "幹");
}

- (void)test_Utilities_PinyinToPhonaCompound {
  XCTAssertEqual(cnvHanyuPinyinToPhona("shang4"), "ㄕㄤˋ");
  XCTAssertEqual(cnvHanyuPinyinToPhona("zhang1"), "ㄓㄤ");
  XCTAssertEqual(cnvHanyuPinyinToPhona("zhang1", " "), "ㄓㄤ ");
  // 含不允許字元（非半形英數）時放棄轉換、原樣回傳。
  XCTAssertEqual(cnvHanyuPinyinToPhona("nǐ"), "nǐ");
}

- (void)test_Utilities_MakeToneInsensitiveVariantsBasic {
  // 無調讀音展開為同音節聲調候選桶：陰平以空字串表示，
  // 順序依 allowedIntonations（" ", "ˊ", "ˇ", "ˋ", "˙"）。
  std::vector<std::string> expected = {"ㄕ", "ㄕˊ", "ㄕˇ", "ㄕˋ", "ㄕ˙"};
  auto result = makeToneInsensitiveVariants("ㄕ");
  XCTAssertTrue(result == expected);
  // 去重守衛：回傳內容不得重複。
  XCTAssertEqual(result.size(), allowedIntonations.size());
  std::set<std::string> dedup(result.begin(), result.end());
  XCTAssertEqual(dedup.size(), result.size());
}

- (void)test_Utilities_MakeToneInsensitiveVariantsEmptyReading {
  std::vector<std::string> expected = {"", "ˊ", "ˇ", "ˋ", "˙"};
  XCTAssertTrue(makeToneInsensitiveVariants("") == expected);
}

- (void)test_Utilities_HasStringEdgeCases {
  XCTAssertTrue(stringInclusion("ㄅㄧㄢˋ", "ㄧㄢ"));
  XCTAssertFalse(stringInclusion("ㄅㄧㄢˋ", "ㄧㄥ"));
  XCTAssertTrue(stringInclusion("aaa", "aa"));
  XCTAssertFalse(stringInclusion("x", "xyz"));
  // 空目標的既有語義：僅當自身為空時為 true。
  XCTAssertTrue(stringInclusion("", ""));
  XCTAssertFalse(stringInclusion("a", ""));
  XCTAssertFalse(stringInclusion("", "a"));
}

- (void)test_Utilities_SwappingEdgeCases {
  // replaceOccurrences 為原地改寫，故逐條複製字串後再施作。
  auto swapped = [](std::string data, const std::string& target,
                    const std::string& replacement) {
    replaceOccurrences(data, target, replacement);
    return data;
  };
  XCTAssertEqual(swapped("a-b-c", "-", "+"), "a+b+c");
  XCTAssertEqual(swapped("ㄅㄧㄢ", "ㄧㄢ", "ian"), "ㄅian");
  // 空替換內容等同於刪除目標。
  XCTAssertEqual(swapped("a-b-c", "-", ""), "abc");
  // 空目標的既有語義：原樣回傳自身。
  XCTAssertEqual(swapped("abc", "", "x"), "abc");
  // 目標自體重疊時採不重疊比對：自左向右、命中即跳過整段目標。
  XCTAssertEqual(swapped("aaaa", "aa", "b"), "bb");
  XCTAssertEqual(swapped("aaa", "aa", "b"), "ba");
  // 無命中時原樣回傳。
  XCTAssertEqual(swapped("abc", "xyz", "b"), "abc");
}

@end
