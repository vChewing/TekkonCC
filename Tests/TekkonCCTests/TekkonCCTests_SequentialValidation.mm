// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

// 循序輸入檢定（`isSequentiallyTypedRawKeyOrder`）之五支測項。

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#import "../TestAssets_Tekkon/TekkonTestData.hh"
#import "Tekkon.hh"

using namespace Tekkon;

namespace {

/// 一筆序列檢定測資：注音排列、鍵入序列、期望結果。
struct SequentialValidationTestCase {
  MandarinParser parser;
  std::string input;
  bool expected;
};

/// 依空白切分（連續空白視為單一分隔、不產生空 token）。
std::vector<std::string> splitByWhitespace(const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream stream(line);
  std::string token;
  while (stream >> token) tokens.emplace_back(token);
  return tokens;
}

/// 將底線還原為空格（語料表以底線代表空白）。
std::string replaceUnderscores(const std::string& str) {
  std::string result = str;
  std::replace(result.begin(), result.end(), '_', ' ');
  return result;
}

/// 組字器之所有可觀測狀態（供「呼叫不改自身」比對用）。
std::vector<std::string> observableState(Composer& composer) {
  return {composer.consonant.value(),
          composer.semivowel.value(),
          composer.vowel.value(),
          composer.intonation.value(),
          composer.romajiBuffer,
          std::to_string(static_cast<int>(composer.parser)),
          composer.enforceCSVTOrdering ? "1" : "0",
          composer.phonabetCombinationCorrectionEnabled ? "1" : "0",
          composer.allowsExtendedRomajiBuffer ? "1" : "0"};
}

}  // namespace

@interface TekkonCCTests_SequentialValidation : XCTestCase

@end

@implementation TekkonCCTests_SequentialValidation

// MARK: - 靜態注音排列

- (void)test_SequentialValidation_StaticLayouts {
  // 靜態注音排列：逐鍵即為逐個注音，依序鍵入者恆應合格。首六筆皆為「ㄅㄚˇ」。
  const std::vector<SequentialValidationTestCase> cases = {
      {ofDachen, "183", true},  {ofETen, "ba3", true},
      {ofIBM, "1f,", true},     {ofMiTAC, "ba3", true},
      {ofSeigyou, "28a", true}, {ofFakeSeigyou, "28a", true},
      {ofDachen, "cl", true},    // ㄏㄠ
      {ofDachen, "1u,", true},   // ㄅㄧㄝ
      {ofDachen, "5j/ ", true},  // ㄓㄨㄥ
      {ofDachen, "xup6", true},  // ㄌㄧㄣˊ
      {ofDachen, "hl3", true},   // ㄘㄠˇ
      {ofDachen, "ll", true},    // 同值之重複寫入不留痕跡、不構成違序
      {ofDachen, "lc", false},   // 先韻後聲：倒序
      {ofDachen, "us", false},   // 先介後聲：倒序
      {ofDachen, "3u", false},   // 聲調前置
      {ofDachen, "44", false},   // 僅聲調、不可唸
      {ofDachen, "C", false},    // 非該排列之按鍵
      {ofDachen, "幹", false},   // 非按鍵
      {ofDachen, "", false},     // 空序列
      {ofDachen, " ", false},    // 僅陰平
  };
  for (const auto& testCase : cases) {
    Composer composer("", testCase.parser);
    BOOL result = composer.isSequentiallyTypedRawKeyOrder(testCase.input);
    XCTAssertEqual(result, testCase.expected ? YES : NO,
                   @"parser#%d \"%s\" 應為 %@",
                   static_cast<int>(testCase.parser), testCase.input.c_str(),
                   testCase.expected ? @"true" : @"false");
  }
}

// MARK: - 覆寫修正與 suffixOnly

/// 覆寫修正（以另一鍵改寫既有槽值）於預設模式下判不合格；suffixOnly
/// 模式下放寬之。
- (void)testRawKeyOrder_OverwriteCorrectionAndSuffixOnly {
  struct OverwriteTestCase {
    MandarinParser parser;
    std::string input;
    bool expectedStrict;
    bool expectedSuffixOnly;
  };
  const std::vector<OverwriteTestCase> cases = {
      {ofDachen, "cl", true, true},
      {ofDachen, "dcl", false, true},    // ㄎ→ㄏ 以另一鍵覆寫修正
      {ofDachen, "qn", false, true},     // ㄆ→ㄙ 以另一鍵覆寫修正
      {ofDachen, "cl34", false, true},   // 聲調 ˇ→ˋ 以另一鍵覆寫
      {ofDachen, "ll", true, true},      // 同值重寫：無可觀測變化
      {ofDachen, "lc", false, false},    // 倒序：兩模式皆不合格
      {ofDachen26, "qquu", true, true},  // 同鍵重寫：排列自身編碼所必需
      {ofDachen26, "uuu", true, true},
      {ofDachen26, "mm", true, true},
      {ofETen26, "ge", true,
       true},  // ㄓ→ㄐ：引擎自身之跨鍵糾正（條件五不在其限）
      {ofHanyuPinyin, "su3", true, true},
      {ofHanyuPinyin, "suan", true, true},
      {ofHanyuPinyin, "3su", false, false},
  };
  for (const auto& testCase : cases) {
    Composer composer("", testCase.parser);
    XCTAssertEqual(composer.isSequentiallyTypedRawKeyOrder(testCase.input),
                   testCase.expectedStrict);
    XCTAssertEqual(
        composer.isSequentiallyTypedRawKeyOrder(testCase.input, true),
        testCase.expectedSuffixOnly);
  }
}

// MARK: - 動態注音排列（典型案例）

- (void)test_SequentialValidation_DynamicLayoutsTypical {
  const std::vector<SequentialValidationTestCase> cases = {
      {ofDachen26, "qquu", true},    // ㄅㄚ（首擊為ㄆ、次擊覆寫為ㄅ）
      {ofDachen26, "qquur", true},   // ㄅㄚˇ
      {ofDachen26, "uuu", true},     // ㄧㄚ（末擊補回介母）
      {ofDachen26, "mm", true},      // ㄩ（首擊為ㄡ、次擊覆寫為ㄩ）
      {ofDachen26, "uuqq", false},   // 亂序
      {ofDachen26, "qqruu", false},  // 聲調前置
      {ofETen26, "baj", true},       // ㄅㄚˇ
      {ofHsu, "byf", true},          // ㄅㄚˇ
      {ofStarlight, "ba8", true},    // ㄅㄚˇ
      {ofAlvinLiu, "baj", true},     // ㄅㄚˇ
  };
  for (const auto& testCase : cases) {
    Composer composer("", testCase.parser);
    BOOL result = composer.isSequentiallyTypedRawKeyOrder(testCase.input);
    XCTAssertEqual(result, testCase.expected ? YES : NO,
                   @"parser#%d \"%s\" 應為 %@",
                   static_cast<int>(testCase.parser), testCase.input.c_str(),
                   testCase.expected ? @"true" : @"false");
  }
}

// MARK: - 動態注音排列（語料全表）

- (void)test_SequentialValidation_DynamicLayoutsCorpus {
  const std::vector<MandarinParser> parserOrder = {ofDachen26, ofETen26, ofHsu,
                                                   ofStarlight, ofAlvinLiu};
  std::string testDataCpp = TekkonTestData::testTable4DynamicLayouts;
  std::vector<std::vector<std::string>> typings(parserOrder.size());

  std::istringstream lineStream(testDataCpp);
  std::string line;
  while (std::getline(lineStream, line)) {
    if (line.empty()) continue;
    if (line[0] == '$') continue;  // 表頭列（$READING ...）。
    std::vector<std::string> cells = splitByWhitespace(line);
    if (cells.size() <= parserOrder.size())
      continue;  // 需要 expected + 5 個排列欄位。
    for (size_t index = 0; index < parserOrder.size(); ++index) {
      // 欄位索引：0=expected, 1=Dachen26, 2=ETen26, 3=Hsu, 4=Starlight,
      // 5=AlvinLiu。
      std::string typing = replaceUnderscores(cells[index + 1]);
      if (!typing.empty() && typing[0] == '`') continue;
      typings[index].emplace_back(typing);
    }
  }

  for (size_t index = 0; index < parserOrder.size(); ++index) {
    std::vector<std::string> rejected;
    for (const auto& typing : typings[index]) {
      Composer composer("", parserOrder[index]);
      if (!composer.isSequentiallyTypedRawKeyOrder(typing, true))
        rejected.emplace_back(typing);
    }
    if (!rejected.empty()) {
      NSLog(@" -> parser#%d 有 %d 筆合法編碼未被接受：%@",
            static_cast<int>(parserOrder[index]),
            static_cast<int>(rejected.size()),
            [NSString stringWithUTF8String:rejected[0].c_str()]);
    }
    XCTAssertTrue(rejected.empty(), @"parser#%d 有 %d 筆合法編碼未被接受",
                  static_cast<int>(parserOrder[index]),
                  static_cast<int>(rejected.size()));
  }
}

// MARK: - 拼音排列

- (void)test_SequentialValidation_PinyinLayouts {
  const std::vector<SequentialValidationTestCase> cases = {
      {ofHanyuPinyin, "su3", true},         {ofHanyuPinyin, "suan", true},
      {ofHanyuPinyin, "suan3", true},       {ofHanyuPinyin, "su", true},
      {ofHanyuPinyin, "su ", true},         {ofHanyuPinyin, "zhong", true},
      {ofHanyuPinyin, "shi", true},         {ofHanyuPinyin, "nv", true},
      {ofHanyuPinyin, "3su", false},  // 聲調前置、為引擎所丟棄
      {ofHanyuPinyin, "s u", false},  // 聲調夾於字中、同遭丟棄
      {ofHanyuPinyin, "sh", false},   // 未成音節
      {ofHanyuPinyin, "S", false},    // 大寫非該排列之按鍵
      {ofHanyuPinyin, "hello", false},      {ofHanyuPinyin, "us", false},
      {ofSecondaryPinyin, "chiung2", true}, {ofSecondaryPinyin, "zhong", false},
      {ofYalePinyin, "jung", true},         {ofYalePinyin, "suan", false},
      {ofHualuoPinyin, "suan", true},       {ofUniversalPinyin, "suan", true},
      {ofWadeGilesPinyin, "jung", true},
  };
  for (const auto& testCase : cases) {
    Composer composer("", testCase.parser);
    BOOL result = composer.isSequentiallyTypedRawKeyOrder(testCase.input);
    XCTAssertEqual(result, testCase.expected ? YES : NO,
                   @"parser#%d \"%s\" 應為 %@",
                   static_cast<int>(testCase.parser), testCase.input.c_str(),
                   testCase.expected ? @"true" : @"false");
  }
}

// MARK: - 呼叫不改自身

- (void)test_SequentialValidation_DoesNotMutateComposer {
  Composer composer("", ofDachen);
  composer.receiveSequence("hl3");
  std::vector<std::string> snapshot = observableState(composer);
  XCTAssertTrue(composer.isSequentiallyTypedRawKeyOrder("1u,"));
  XCTAssertTrue(observableState(composer) == snapshot);
  // 呼叫端之 CSVT 順序強制設定不應影響本 API 之判定（槽序由本 API 自行觀測）。
  composer.enforceCSVTOrdering = true;
  snapshot = observableState(composer);
  XCTAssertTrue(composer.isSequentiallyTypedRawKeyOrder("dcl", true));
  XCTAssertTrue(observableState(composer) == snapshot);
}

@end
