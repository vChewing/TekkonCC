// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// 對應 Swift 版 Tests/TekkonTests/TekkonTests_SyllableIndex.swift 之九支測項，
// 暨 TekkonTests_Pinyin.swift 之
// <PinyinSingleLetterEntries_AreRealSyllablesOnly>。
//
// 索引之語意與兩條紅線見 Sources/Tekkon/include/Tekkon.hh 之 SyllableIndex
// 說明。 本檔之斷言分三類：① 資料規模（426／442／16／37 四項可稽核數字）；②
// 成員資格之正反例； ③ 與引擎既有表（allowedConsonants
// 等）及測試素材之交叉比對。

#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../Sources/Tekkon/include/Tekkon.hh"
#include "../Tests/TestAssets_Tekkon/TekkonTestData.hh"
#include "gtest/gtest.h"

namespace Tekkon {

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

std::string vowelKeyToComposition(char key) {
  Composer composer("", ofHanyuPinyin);
  composer.receiveKey(std::string(1, key));
  return composer.getComposition();
}

}  // namespace

// MARK: 資料規模

TEST(TekkonTests_SyllableIndex, CanonicalReadingsMatchThePinyinMap) {
  auto& index = SyllableIndex::shared(ofDachen);
  EXPECT_EQ(index.readings().size(), 426u);
  EXPECT_EQ(SyllableIndex::allReadings(), index.readings());
  std::set<std::string> expected;
  for (const auto& pair : mapHanyuPinyin) expected.insert(pair.second);
  std::set<std::string> actual(index.readings().begin(),
                               index.readings().end());
  EXPECT_EQ(actual, expected);
  // 升冪、且無重複。
  EXPECT_TRUE(std::is_sorted(index.readings().begin(), index.readings().end()));
  EXPECT_EQ(actual.size(), index.readings().size());
}

TEST(TekkonTests_SyllableIndex, PrefixSetIsExactlyFourHundredFortyTwo) {
  auto& index = SyllableIndex::shared(ofDachen);
  auto derived = derivedPrefixes(index.readings());
  EXPECT_EQ(derived.size(), 442u);
  // 每一條推導而得之前綴皆須被索引認可。
  for (const auto& prefix : derived) {
    EXPECT_TRUE(index.isPrefix(prefix)) << prefix;
  }
  // 必然為假者。
  for (const std::string& bogus :
       {std::string(""), std::string("ㄍㄋ"), std::string("ㄅㄆ"),
        std::string("ㄚㄅ"), std::string("abc"), std::string("zhi"),
        std::string("ㄅㄚˇ"), std::string("ㄅㄚˋ"), std::string("ㄅㄚ1"),
        std::string(" ㄅ")}) {
    EXPECT_FALSE(index.isPrefix(bogus)) << bogus;
  }
  // 逐條讀音之延伸一符號後皆假。
  for (const auto& reading : index.readings()) {
    EXPECT_TRUE(index.isPrefix(reading));
    EXPECT_FALSE(index.isPrefix(reading + "ㄅ"));
  }
}

TEST(TekkonTests_SyllableIndex, OneCharacterPrefixesAreThePhonabetTables) {
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
  EXPECT_EQ(derivedOneChar.size(), 37u);
  EXPECT_EQ(derivedOneChar, tableUnion);
  for (const auto& symbol : tableUnion) EXPECT_TRUE(index.isPrefix(symbol));
}

TEST(TekkonTests_SyllableIndex, StrictPrefixesAreExactlyTheSixteen) {
  auto& index = SyllableIndex::shared(ofDachen);
  auto derived = derivedPrefixes(index.readings());
  std::vector<std::string> strict;
  for (const auto& prefix : derived) {
    if (!index.isComplete(prefix)) strict.push_back(prefix);
  }
  const std::vector<std::string> expected = {
      "ㄅ", "ㄆ", "ㄇ", "ㄈ",   "ㄈㄧ", "ㄉ", "ㄊ", "ㄋ",
      "ㄌ", "ㄍ", "ㄎ", "ㄎㄧ", "ㄏ",   "ㄐ", "ㄑ", "ㄒ"};
  EXPECT_EQ(strict, expected);
  for (const auto& symbol : strict) {
    EXPECT_TRUE(index.isPrefix(symbol)) << symbol;
    EXPECT_FALSE(index.isComplete(symbol)) << symbol;
  }
  // 反向：推導集之中，凡不在上述 16 條者皆須為完整讀音。
  std::set<std::string> strictSet(strict.begin(), strict.end());
  for (const auto& prefix : derived) {
    if (strictSet.count(prefix)) continue;
    EXPECT_TRUE(index.isComplete(prefix)) << prefix;
  }
  // 兩條 2 字嚴格前綴之來由。
  EXPECT_EQ(index.completions("ㄈㄧ"), (std::vector<std::string>{"ㄈㄧㄠ"}));
  EXPECT_EQ(index.completions("ㄎㄧ"),
            (std::vector<std::string>{"ㄎㄧㄡ", "ㄎㄧㄤ"}));
}

TEST(TekkonTests_SyllableIndex, SingleSymbolCompleteSplit) {
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
  EXPECT_EQ(completeOneChar.size(), 23u);
  const std::vector<std::string> expectedStrict = {"ㄅ", "ㄆ", "ㄇ", "ㄈ", "ㄉ",
                                                   "ㄊ", "ㄋ", "ㄌ", "ㄍ", "ㄎ",
                                                   "ㄏ", "ㄐ", "ㄑ", "ㄒ"};
  EXPECT_EQ(strictOneChar, expectedStrict);
  // 預期之完整單符號：7 個可獨立成音節之聲母 ＋ 3 個介母 ＋ 13 個韻母。
  std::set<std::string> expectedComplete = {
      "ㄓ", "ㄔ", "ㄕ", "ㄖ", "ㄗ", "ㄘ", "ㄙ", "ㄚ", "ㄛ", "ㄜ", "ㄝ", "ㄞ",
      "ㄟ", "ㄠ", "ㄡ", "ㄢ", "ㄣ", "ㄤ", "ㄥ", "ㄦ", "ㄧ", "ㄨ", "ㄩ"};
  EXPECT_EQ(
      std::set<std::string>(completeOneChar.begin(), completeOneChar.end()),
      expectedComplete);
  // `ㄑ` 不是獨立音節，只以「ㄑ 一族之嚴格前綴」之身分存在。
  EXPECT_FALSE(index.isComplete("ㄑ"));
  EXPECT_TRUE(index.isPrefix("ㄑ"));
  EXPECT_FALSE(index.completions("ㄑ").empty());
  // 單字母條目僅餘三條真音節。
  EXPECT_EQ(mapHanyuPinyin.count("qi"), 1u);
  EXPECT_EQ(mapHanyuPinyin.at("qi"), "ㄑㄧ");
  EXPECT_EQ(mapHanyuPinyin.count("q"), 0u);
  EXPECT_EQ(mapHanyuPinyin.count("b"), 0u);
  EXPECT_EQ(mapHanyuPinyin.count("p"), 0u);
  std::set<std::string> singleLetterKeys;
  for (const auto& pair : mapHanyuPinyin) {
    if (pair.first.size() == 1) singleLetterKeys.insert(pair.first);
  }
  EXPECT_EQ(singleLetterKeys, (std::set<std::string>{"a", "e", "o"}));
  EXPECT_EQ(mapHanyuPinyin.at("a"), "ㄚ");
  EXPECT_EQ(mapHanyuPinyin.at("e"), "ㄜ");
  EXPECT_EQ(mapHanyuPinyin.at("o"), "ㄛ");
  EXPECT_EQ(mapHanyuPinyin.size(), 426u);
}

// MARK: 成員資格

TEST(TekkonTests_SyllableIndex, CompleteAlwaysImpliesPrefix) {
  auto& index = SyllableIndex::shared(ofDachen);
  for (const auto& reading : index.readings()) {
    EXPECT_TRUE(index.isComplete(reading));
    EXPECT_TRUE(index.isPrefix(reading));
  }
  for (const auto& reading : SyllableIndex::allReadings()) {
    EXPECT_TRUE(index.isComplete(reading));
  }
}

TEST(TekkonTests_SyllableIndex, CompletionsAreSortedAndStable) {
  auto& index = SyllableIndex::shared(ofDachen);
  // 完整讀音：僅回傳自身。
  EXPECT_EQ(index.completions("ㄍㄚ"), (std::vector<std::string>{"ㄍㄚ"}));
  // 空字串：全部 426 條。此與 isPrefix("") == false
  // 並不矛盾——後者是「非空」之定義， 前者是列舉之定義。
  EXPECT_EQ(index.completions("").size(), 426u);
  EXPECT_FALSE(index.isPrefix(""));
  // 非前綴：空集。
  EXPECT_TRUE(index.completions("ㄍㄋ").empty());
  EXPECT_TRUE(index.completions("zzz").empty());
  // 升冪、且與兩次呼叫之結果一致。
  for (const std::string& prefix : {"ㄍ", "ㄓ", "ㄧ", "ㄈㄧ"}) {
    auto result = index.completions(prefix);
    EXPECT_FALSE(result.empty()) << prefix;
    EXPECT_TRUE(std::is_sorted(result.begin(), result.end()));
    EXPECT_EQ(result, index.completions(prefix));
    for (const auto& reading : result) {
      EXPECT_EQ(reading.compare(0, prefix.size(), prefix), 0);
    }
  }
  EXPECT_GT(index.completions("ㄍ").size(), 1u);
}

TEST(TekkonTests_SyllableIndex, TestTableStemsAreAllCompleteReadings) {
  auto& index = SyllableIndex::shared(ofDachen);
  auto stems = testTableStems();
  EXPECT_EQ(stems.size(), 422u);
  // 422 條之中，421 條為完整讀音；唯一之例外是 `ㄑ`。
  std::set<std::string> notComplete;
  for (const auto& stem : stems) {
    if (!index.isComplete(stem)) notComplete.insert(stem);
  }
  EXPECT_EQ(notComplete, (std::set<std::string>{"ㄑ"}));
  // 反向落差：字典有而素材無者恰為 5 條。
  std::set<std::string> readings(index.readings().begin(),
                                 index.readings().end());
  std::set<std::string> onlyInMap;
  for (const auto& reading : readings) {
    if (!stems.count(reading)) onlyInMap.insert(reading);
  }
  EXPECT_EQ(onlyInMap, (std::set<std::string>{"ㄈㄨㄥ", "ㄍㄧ", "ㄍㄨㄜ",
                                              "ㄎㄧㄡ", "ㄘㄟ"}));
}

// MARK: 共用快取

TEST(TekkonTests_SyllableIndex, SharedIndexIsParserNeutral) {
  const std::vector<MandarinParser> parsers = {
      ofDachen, ofDachen26, ofETen, ofHanyuPinyin, ofWadeGilesPinyin};
  std::vector<std::vector<std::string>> snapshots;
  for (MandarinParser parser : parsers) {
    snapshots.push_back(SyllableIndex::shared(parser).readings());
  }
  for (const auto& snapshot : snapshots) {
    EXPECT_EQ(snapshot.size(), 426u);
    EXPECT_EQ(snapshot, snapshots.front());
  }
  // 清快取之後仍可重建，且內容不變。
  SyllableIndex::clearSharedCache();
  auto& rebuilt = SyllableIndex::shared(ofDachen);
  EXPECT_EQ(rebuilt.readings(), snapshots.front());
  EXPECT_TRUE(rebuilt.isPrefix("ㄍ"));
  EXPECT_FALSE(rebuilt.isPrefix("ㄍㄋ"));
  EXPECT_TRUE(rebuilt.isComplete("ㄍㄚ"));
  EXPECT_FALSE(rebuilt.isComplete("ㄍ"));
}

// MARK: 單字母條目之解碼契約

TEST(TekkonTests_SyllableIndex, HanyuPinyinSingleLetterEntries) {
  // `q` 與 `b`／`z` 皆不寫槽（整體查表無此條目）；`a`／`e`／`o`
  // 則寫入各自之韻母。
  for (const std::string& key : {"q", "b", "z"}) {
    Composer composer("", ofHanyuPinyin);
    composer.receiveKey(key);
    EXPECT_EQ(composer.getComposition(), "") << key;
    EXPECT_EQ(composer.romajiBuffer, key);
  }
  EXPECT_EQ(vowelKeyToComposition('a'), "ㄚ");
  EXPECT_EQ(vowelKeyToComposition('e'), "ㄜ");
  EXPECT_EQ(vowelKeyToComposition('o'), "ㄛ");

  // 狂拼之自動切音節：`q`＋`f` 與 `b`＋`f` 皆不提交（nullopt）。
  for (const std::string& first : {"q", "b", "z"}) {
    Composer composer("", ofHanyuPinyin);
    composer.allowsExtendedRomajiBuffer = true;
    composer.receiveKey(first);
    composer.romajiBuffer = first;
    EXPECT_FALSE(composer.pinyinAutoChopResult("f").has_value()) << first;
  }
  // 對照組：`a`＋`f` 仍會提交 ㄚ（`a` 是真音節，此行為由音節事實支持）。
  {
    Composer composer("", ofHanyuPinyin);
    composer.allowsExtendedRomajiBuffer = true;
    composer.receiveKey("a");
    composer.romajiBuffer = "a";
    auto chopped = composer.pinyinAutoChopResult("f");
    ASSERT_TRUE(chopped.has_value());
    EXPECT_EQ(chopped->committedReadings, (std::vector<std::string>{"ㄚ"}));
  }

  // 拼音片段展開：`q` 與 `b` 皆走前綴擴張（不再是精確命中）。
  auto& trie = PinyinTrie::shared(ofHanyuPinyin);
  auto qExpansion = trie.zhuyinReadings("q");
  EXPECT_EQ(std::count(qExpansion.begin(), qExpansion.end(), "ㄑ"), 0);
  EXPECT_GT(std::count(qExpansion.begin(), qExpansion.end(), "ㄑㄧ"), 0);
  EXPECT_GT(qExpansion.size(), 1u);
  EXPECT_GT(trie.zhuyinReadings("b").size(), 1u);
  // `a` 仍是精確命中。
  EXPECT_EQ(trie.zhuyinReadings("a"), (std::vector<std::string>{"ㄚ"}));
}

}  // namespace Tekkon
