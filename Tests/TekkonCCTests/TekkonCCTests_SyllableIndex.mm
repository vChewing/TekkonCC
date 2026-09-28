// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

// 讀音前綴索引（`SyllableIndex`）之九支測項，暨漢語拼音單字母條目之解碼契約。
// 與 GTests/TekkonTests_SyllableIndex.cc 內容雷同，僅測試框架不同。

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#import "../TestAssets_Tekkon/TekkonTestData.hh"
#import "Tekkon.hh"

using namespace Tekkon;

namespace {

/// 由全部讀音逐條展開之全部非空前綴（以 UTF-8 碼點為界）。
std::set<std::string> derivedPrefixes(
    const std::vector<std::string>& readings) {
  std::set<std::string> result;
  for (const auto& reading : readings) {
    std::string accumulated;
    for (const auto& codepoint : splitByCodepoint(reading)) {
      accumulated += codepoint;
      result.insert(accumulated);
    }
  }
  return result;
}

/// 將底線還原為空格（語料表以底線代表空白）。
std::string replaceUnderscores(const std::string& str) {
  std::string result = str;
  std::replace(result.begin(), result.end(), '_', ' ');
  return result;
}

/// 語料表之全部無調詞幹（底線＝空格＝陰平，須先還原再剝調）。
std::set<std::string> testTableStems() {
  std::set<std::string> stems;
  std::istringstream stream(TekkonTestData::testTable4DynamicLayouts);
  std::string line;
  while (std::getline(stream, line)) {
    std::istringstream lineStream(line);
    std::string firstToken;
    if (!(lineStream >> firstToken)) continue;
    // 表頭（`$READING Dachen26 …`）非測資。
    if (firstToken.rfind('$', 0) == 0) continue;
    std::string reading = replaceUnderscores(firstToken);
    if (!reading.empty()) {
      auto codepoints = splitByCodepoint(reading);
      if (!codepoints.empty()) {
        const std::string& last = codepoints.back();
        if (last == " " || last == "ˊ" || last == "ˇ" || last == "ˋ" ||
            last == "˙") {
          reading.clear();
          for (size_t i = 0; i + 1 < codepoints.size(); ++i) {
            reading += codepoints[i];
          }
        }
      }
    }
    stems.insert(reading);
  }
  return stems;
}

/// 由單一讀音取首個碼點（＝單符號前綴）。
std::string firstCodepoint(const std::string& reading) {
  auto codepoints = splitByCodepoint(reading);
  return codepoints.empty() ? std::string() : codepoints.front();
}

}  // namespace

@interface TekkonCCTests_SyllableIndex : XCTestCase

@end

@implementation TekkonCCTests_SyllableIndex

// =========== SYLLABLE INDEX TESTS ===========

- (void)test_SyllableIndex_CanonicalReadingsMatchThePinyinMap {
  auto& index = SyllableIndex::shared(ofDachen);
  XCTAssertEqual(index.readings().size(), 426UL);
  XCTAssertTrue(SyllableIndex::allReadings() == index.readings());
  std::set<std::string> expected;
  for (const auto& pair : mapHanyuPinyin) expected.insert(pair.second);
  std::set<std::string> actual(index.readings().begin(),
                               index.readings().end());
  XCTAssertTrue(actual == expected);
  // 升冪、且無重複。
  XCTAssertTrue(
      std::is_sorted(index.readings().begin(), index.readings().end()));
  XCTAssertEqual(actual.size(), index.readings().size());
}

- (void)test_SyllableIndex_PrefixSetIsExactlyFourHundredFortyTwo {
  auto& index = SyllableIndex::shared(ofDachen);
  auto derived = derivedPrefixes(index.readings());
  XCTAssertEqual(derived.size(), 442UL);
  // 每一條推導而得之前綴皆須被索引認可。
  for (const auto& prefix : derived) XCTAssertTrue(index.isPrefix(prefix));
  // 必然為假者。
  for (const std::string& bogus :
       {std::string(""), std::string("ㄍㄋ"), std::string("ㄅㄆ"),
        std::string("ㄚㄅ"), std::string("abc"), std::string("zhi"),
        std::string("ㄅㄚˇ"), std::string("ㄅㄚˋ"), std::string("ㄅㄚ1"),
        std::string(" ㄅ")}) {
    XCTAssertFalse(index.isPrefix(bogus));
  }
  // 逐條讀音之延伸一符號後皆假。
  for (const auto& reading : index.readings()) {
    XCTAssertTrue(index.isPrefix(reading));
    XCTAssertFalse(index.isPrefix(reading + "ㄅ"));
  }
}

- (void)test_SyllableIndex_OneCharacterPrefixesAreThePhonabetTables {
  auto& index = SyllableIndex::shared(ofDachen);
  std::set<std::string> derivedOneChar;
  for (const auto& reading : index.readings()) {
    derivedOneChar.insert(firstCodepoint(reading));
  }
  std::set<std::string> tableUnion;
  for (char32_t scalar : allowedConsonants) {
    tableUnion.insert(char32ToString(scalar));
  }
  for (char32_t scalar : allowedSemivowels) {
    tableUnion.insert(char32ToString(scalar));
  }
  for (char32_t scalar : allowedVowels) {
    tableUnion.insert(char32ToString(scalar));
  }
  XCTAssertEqual(derivedOneChar.size(), 37UL);
  XCTAssertTrue(derivedOneChar == tableUnion);
  for (const auto& symbol : tableUnion) XCTAssertTrue(index.isPrefix(symbol));
}

- (void)test_SyllableIndex_StrictPrefixesAreExactlyTheSixteen {
  auto& index = SyllableIndex::shared(ofDachen);
  auto derived = derivedPrefixes(index.readings());
  std::vector<std::string> strict;
  for (const auto& prefix : derived) {
    if (!index.isComplete(prefix)) strict.push_back(prefix);
  }
  const std::vector<std::string> expected = {
      "ㄅ", "ㄆ", "ㄇ", "ㄈ",   "ㄈㄧ", "ㄉ", "ㄊ", "ㄋ",
      "ㄌ", "ㄍ", "ㄎ", "ㄎㄧ", "ㄏ",   "ㄐ", "ㄑ", "ㄒ"};
  XCTAssertTrue(strict == expected);
  for (const auto& symbol : strict) {
    XCTAssertTrue(index.isPrefix(symbol));
    XCTAssertFalse(index.isComplete(symbol));
  }
  // 反向：推導集之中，凡不在上述 16 條者皆須為完整讀音。
  std::set<std::string> strictSet(strict.begin(), strict.end());
  for (const auto& prefix : derived) {
    if (strictSet.count(prefix)) continue;
    XCTAssertTrue(index.isComplete(prefix));
  }
  // 兩條 2 字嚴格前綴之來由。
  XCTAssertTrue(index.completions("ㄈㄧ") ==
                (std::vector<std::string>{"ㄈㄧㄠ"}));
  XCTAssertTrue(index.completions("ㄎㄧ") ==
                (std::vector<std::string>{"ㄎㄧㄡ", "ㄎㄧㄤ"}));
}

- (void)test_SyllableIndex_SingleSymbolCompleteSplit {
  auto& index = SyllableIndex::shared(ofDachen);
  std::set<std::string> oneChar;
  for (const auto& reading : index.readings()) {
    oneChar.insert(firstCodepoint(reading));
  }
  std::vector<std::string> completeOneChar;
  std::vector<std::string> strictOneChar;
  for (const auto& symbol : oneChar) {
    (index.isComplete(symbol) ? completeOneChar : strictOneChar)
        .push_back(symbol);
  }
  // 37 ＝ 23 完整 ＋ 14 嚴格。
  XCTAssertEqual(completeOneChar.size(), 23UL);
  const std::vector<std::string> expectedStrict = {"ㄅ", "ㄆ", "ㄇ", "ㄈ", "ㄉ",
                                                   "ㄊ", "ㄋ", "ㄌ", "ㄍ", "ㄎ",
                                                   "ㄏ", "ㄐ", "ㄑ", "ㄒ"};
  XCTAssertTrue(strictOneChar == expectedStrict);
  // 預期之完整單符號：7 個可獨立成音節之聲母 ＋ 3 個介母 ＋ 13 個韻母。
  std::set<std::string> expectedComplete = {
      "ㄓ", "ㄔ", "ㄕ", "ㄖ", "ㄗ", "ㄘ", "ㄙ", "ㄚ", "ㄛ", "ㄜ", "ㄝ", "ㄞ",
      "ㄟ", "ㄠ", "ㄡ", "ㄢ", "ㄣ", "ㄤ", "ㄥ", "ㄦ", "ㄧ", "ㄨ", "ㄩ"};
  XCTAssertTrue(
      std::set<std::string>(completeOneChar.begin(), completeOneChar.end()) ==
      expectedComplete);
  // `ㄑ` 不是獨立音節，只以「ㄑ 一族之嚴格前綴」之身分存在。
  XCTAssertFalse(index.isComplete("ㄑ"));
  XCTAssertTrue(index.isPrefix("ㄑ"));
  XCTAssertFalse(index.completions("ㄑ").empty());
  // 單字母條目僅餘三條真音節。
  XCTAssertEqual(mapHanyuPinyin.count("qi"), 1UL);
  XCTAssertEqual(mapHanyuPinyin.at("qi"), "ㄑㄧ");
  XCTAssertEqual(mapHanyuPinyin.count("q"), 0UL);
  XCTAssertEqual(mapHanyuPinyin.count("b"), 0UL);
  XCTAssertEqual(mapHanyuPinyin.count("p"), 0UL);
  std::set<std::string> singleLetterKeys;
  for (const auto& pair : mapHanyuPinyin) {
    if (pair.first.size() == 1) singleLetterKeys.insert(pair.first);
  }
  XCTAssertTrue(singleLetterKeys == (std::set<std::string>{"a", "e", "o"}));
  XCTAssertEqual(mapHanyuPinyin.at("a"), "ㄚ");
  XCTAssertEqual(mapHanyuPinyin.at("e"), "ㄜ");
  XCTAssertEqual(mapHanyuPinyin.at("o"), "ㄛ");
  XCTAssertEqual(mapHanyuPinyin.size(), 426UL);
}

- (void)test_SyllableIndex_CompleteAlwaysImpliesPrefix {
  auto& index = SyllableIndex::shared(ofDachen);
  for (const auto& reading : index.readings()) {
    XCTAssertTrue(index.isComplete(reading));
    XCTAssertTrue(index.isPrefix(reading));
  }
  for (const auto& reading : SyllableIndex::allReadings()) {
    XCTAssertTrue(index.isComplete(reading));
  }
}

- (void)test_SyllableIndex_CompletionsAreSortedAndStable {
  auto& index = SyllableIndex::shared(ofDachen);
  // 完整讀音：僅回傳自身。
  XCTAssertTrue(index.completions("ㄍㄚ") ==
                (std::vector<std::string>{"ㄍㄚ"}));
  // 空字串：全部 426 條。此與 isPrefix("") == false
  // 並不矛盾——後者是「非空」之定義， 前者是列舉之定義。
  XCTAssertEqual(index.completions("").size(), 426UL);
  XCTAssertFalse(index.isPrefix(""));
  // 非前綴：空集。
  XCTAssertTrue(index.completions("ㄍㄋ").empty());
  XCTAssertTrue(index.completions("zzz").empty());
  // 升冪、且與兩次呼叫之結果一致。
  for (const std::string& prefix : {"ㄍ", "ㄓ", "ㄧ", "ㄈㄧ"}) {
    auto result = index.completions(prefix);
    XCTAssertFalse(result.empty());
    XCTAssertTrue(std::is_sorted(result.begin(), result.end()));
    XCTAssertTrue(result == index.completions(prefix));
    for (const auto& reading : result) {
      XCTAssertEqual(reading.compare(0, prefix.size(), prefix), 0);
    }
  }
  XCTAssertGreaterThan(index.completions("ㄍ").size(), 1UL);
}

- (void)test_SyllableIndex_TestTableStemsAreAllCompleteReadings {
  auto& index = SyllableIndex::shared(ofDachen);
  auto stems = testTableStems();
  XCTAssertEqual(stems.size(), 422UL);
  // 422 條之中，421 條為完整讀音；唯一之例外是 `ㄑ`。
  std::set<std::string> notComplete;
  for (const auto& stem : stems) {
    if (!index.isComplete(stem)) notComplete.insert(stem);
  }
  XCTAssertTrue(notComplete == (std::set<std::string>{"ㄑ"}));
  // 反向落差：字典有而素材無者恰為 5 條。
  std::set<std::string> readings(index.readings().begin(),
                                 index.readings().end());
  std::set<std::string> onlyInMap;
  for (const auto& reading : readings) {
    if (!stems.count(reading)) onlyInMap.insert(reading);
  }
  XCTAssertTrue(onlyInMap == (std::set<std::string>{"ㄈㄨㄥ", "ㄍㄧ", "ㄍㄨㄜ",
                                                    "ㄎㄧㄡ", "ㄘㄟ"}));
}

- (void)test_SyllableIndex_SharedIndexIsParserNeutral {
  const std::vector<MandarinParser> parsers = {
      ofDachen, ofDachen26, ofETen, ofHanyuPinyin, ofWadeGilesPinyin};
  std::vector<std::vector<std::string>> snapshots;
  for (MandarinParser parser : parsers) {
    snapshots.push_back(SyllableIndex::shared(parser).readings());
  }
  for (const auto& snapshot : snapshots) {
    XCTAssertEqual(snapshot.size(), 426UL);
    XCTAssertTrue(snapshot == snapshots.front());
  }
  // 清快取之後仍可重建，且內容不變。
  SyllableIndex::clearSharedCache();
  auto& rebuilt = SyllableIndex::shared(ofDachen);
  XCTAssertTrue(rebuilt.readings() == snapshots.front());
  XCTAssertTrue(rebuilt.isPrefix("ㄍ"));
  XCTAssertFalse(rebuilt.isPrefix("ㄍㄋ"));
  XCTAssertTrue(rebuilt.isComplete("ㄍㄚ"));
  XCTAssertFalse(rebuilt.isComplete("ㄍ"));
}

- (void)test_SyllableIndex_HanyuPinyinSingleLetterEntries {
  // `q` 與 `b`／`z` 皆不寫槽（整體查表無此條目）；`a`／`e`／`o`
  // 則寫入各自之韻母。
  for (const std::string& key : {"q", "b", "z"}) {
    Composer composer = Composer("", ofHanyuPinyin);
    composer.receiveKey(key);
    XCTAssertEqual(composer.getComposition(), "");
    XCTAssertEqual(composer.romajiBuffer, key);
  }
  for (const std::pair<std::string, std::string>& pair :
       {std::pair<std::string, std::string>{"a", "ㄚ"},
        std::pair<std::string, std::string>{"e", "ㄜ"},
        std::pair<std::string, std::string>{"o", "ㄛ"}}) {
    Composer composer = Composer("", ofHanyuPinyin);
    composer.receiveKey(pair.first);
    XCTAssertEqual(composer.getComposition(), pair.second);
  }

  // 自動切音節：`q`＋`f` 與 `b`／`z`＋`f` 皆不提交（nullopt）。
  for (const std::string& first : {"q", "b", "z"}) {
    Composer composer = Composer("", ofHanyuPinyin);
    composer.allowsExtendedRomajiBuffer = true;
    composer.receiveKey(first);
    composer.romajiBuffer = first;
    XCTAssertFalse(composer.pinyinAutoChopResult("f").has_value());
  }
  // 對照組：`a`＋`f` 仍會提交 ㄚ（`a` 是真音節，此行為由音節事實支持）。
  {
    Composer composer = Composer("", ofHanyuPinyin);
    composer.allowsExtendedRomajiBuffer = true;
    composer.receiveKey("a");
    composer.romajiBuffer = "a";
    auto chopped = composer.pinyinAutoChopResult("f");
    XCTAssertTrue(chopped.has_value());
    XCTAssertTrue(chopped->committedReadings ==
                  (std::vector<std::string>{"ㄚ"}));
  }

  // 拼音片段展開：`q` 與 `b` 皆走前綴擴張（不再是精確命中）。
  PinyinTrie& trie = PinyinTrie::shared(ofHanyuPinyin);
  auto qExpansion = trie.zhuyinReadings("q");
  XCTAssertEqual(std::count(qExpansion.begin(), qExpansion.end(), "ㄑ"), 0L);
  XCTAssertGreaterThan(std::count(qExpansion.begin(), qExpansion.end(), "ㄑㄧ"),
                       0L);
  XCTAssertGreaterThan(qExpansion.size(), 1UL);
  XCTAssertGreaterThan(trie.zhuyinReadings("b").size(), 1UL);
  // `a` 仍是精確命中。
  XCTAssertTrue(trie.zhuyinReadings("a") == (std::vector<std::string>{"ㄚ"}));
}

@end
