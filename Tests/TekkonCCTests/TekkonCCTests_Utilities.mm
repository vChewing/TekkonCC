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

- (void)test_Utilities_RestoreToneOneEdgeCases {
  // 空字串防呆（對齊 Swift / C# 版行為）。
  XCTAssertEqual(restoreToneOneInPhona(""), "");
  XCTAssertEqual(restoreToneOneInPhona("ㄉㄧㄠ"), "ㄉㄧㄠ1");
  XCTAssertEqual(restoreToneOneInPhona("ㄉㄧㄠˋ"), "ㄉㄧㄠˋ");
  XCTAssertEqual(restoreToneOneInPhona("ㄉㄧㄠ˙"), "ㄉㄧㄠ˙");
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

@end
