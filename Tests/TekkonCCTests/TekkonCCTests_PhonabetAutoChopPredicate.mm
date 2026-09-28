// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

// 注音狂打自動切音節判準：**生產實作**之回歸靶。
//
// 對應 Swift 版
// Tests/TekkonTests/TekkonTests_PhonabetAutoChopPredicate.swift 之四支測項。與
// GTests/TekkonTests_PhonabetAutoChopPredicate.cc 內容雷同，僅測試框架不同。
//
// 本檔**不**自帶任何「測試端參考實作」——判準之正本只有一處
// （`Composer::shouldAutoChopPhonabets`），本檔直接驅動它，杜絕「兩份各自演化之判準」。
//
// 四項地面真相（與術前驗證之結論逐項對應）：
//   ① 合法單音節編碼之**每一個中途前綴**皆不得觸發切音節；
//   ② 音節交界處**必須**切（殘餘漏切率 < 5%）；
//   ③ 單聲母縮寫（`ess`＝ㄍㄋㄋ）須得三顆鍵；
//   ④ 動態排列之逐槽覆寫（大千26 `qquu`＝ㄅㄚ）之四拍不得被切斷。
//
// 語料（1485 列 × 5 動態排列）**不另抄一份**，而是自
// `Tests/TestAssets_Tekkon/TekkonTestData.hh` 就地解析——故語料仍為單一正本。
//
// 靶之**輸入域**（CI 跟進）：候選鍵與語料單元格皆須落在鍵面字元域內
// （僅 ASCII 字母與數字）。素材檔內之反引號（`` `NULL``）與尾端空格（源自
// `__`）
// 皆為「本排列無此鍵」之標記，**非按鍵**；先前之版本把兩者一併當成候選鍵，
// 遂使反推鍵表把反引號登記成某注音符號之按鍵、由合法讀音之前綴生成出**不可鍵入**
// 之鍵序——而判準對非注音按鍵之反應隨平台而異（Linux 誤切 7366 次、Windows
// 語料整批讀不到）。**此為靶之缺陷，非判準之缺陷**；判準本身未動。
//
// 語料之**載入**（CI 跟進 2）：本倉之語料是編譯期常數 （`TekkonTestData.hh`
// 之原始字串），故 Swift 側該次所加之「候選路徑清單」與
// 「讀不到時附上嘗試紀錄」在本倉**結構上無對位**——檔案根本不會開不成。本倉取該次
// 之兩項可移植者：行尾正規化（Windows checkout 之
// CRLF），以及把「原始列數／過濾後
// 列數」之診斷附於一切依賴語料之斷言（`AutoChopCorpus::report()`）。

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#import "../TestAssets_Tekkon/TekkonTestData.hh"
#import "Tekkon.hh"

using namespace Tekkon;

namespace {

/// 動態排列語料之行（欄序與素材檔同）。
struct AutoChopRow {
  std::string reading;
  std::vector<std::string> cells;
};

/// 解析後之語料：過濾後之資料列 ＋ 素材檔之原始資料列數（**未**過濾）。
struct AutoChopCorpus {
  std::vector<AutoChopRow> rows;
  int rawRowCount = 0;

  /// 診斷訊息（附於一切依賴語料之斷言）。
  ///
  /// Swift
  /// 側之同項尚須列出嘗試過的檔案路徑；本倉之語料是**編譯期常數**，讀不到係
  /// 結構上不可能，故本項只報來源與兩個列數——「沒讀到」與「讀到了但濾掉幾列」在本倉
  /// 只能是後者，而兩者之數值仍須一眼可辨。
  std::string report() const {
    return std::string(
               "語料來源：Tests/TestAssets_Tekkon/TekkonTestData.hh 之 "
               "testTable4DynamicLayouts（編譯期常數，讀不到係結構上不可能）"
               "。") +
           "已解析：raw=" + std::to_string(rawRowCount) +
           "、kept=" + std::to_string(rows.size()) + "。";
  }
};

/// 一種注音排列（名字僅供診斷輸出）。
struct AutoChopLayout {
  std::string name;
  MandarinParser parser;
};

/// 語料表之五個動態排列（欄序與素材檔同）。
const std::vector<AutoChopLayout>& dynamicLayouts() {
  static const std::vector<AutoChopLayout> layouts = {
      {"Dachen26", ofDachen26},
      {"ETen26", ofETen26},
      {"Hsu", ofHsu},
      {"Starlight", ofStarlight},
      {"AlvinLiu", ofAlvinLiu}};
  return layouts;
}

/// 六個靜態排列（一鍵一注音，鍵表可由公開 API 反推）。
const std::vector<AutoChopLayout>& staticLayouts() {
  static const std::vector<AutoChopLayout> layouts = {
      {"Dachen", ofDachen},   {"ETen", ofETen},
      {"IBM", ofIBM},         {"MiTAC", ofMiTAC},
      {"Seigyou", ofSeigyou}, {"FakeSeigyou", ofFakeSeigyou}};
  return layouts;
}

/// std::string（UTF-8）→ NSString。
NSString* nsString(const std::string& text) {
  return [NSString stringWithUTF8String:text.c_str()];
}

/// 將底線還原為空格（語料表以底線代表空白＝陰平鍵）。
std::string replaceUnderscores(const std::string& str) {
  std::string result = str;
  std::replace(result.begin(), result.end(), '_', ' ');
  return result;
}

/// 行尾正規化：CRLF／CR 一律化為 LF。
///
/// 本倉之素材是原始字串常數（`TekkonTestData.hh`）：版控內為 LF，但 Windows 之
/// checkout 可能改寫為 CRLF。此處顯式正規化，免日後之解析器倚賴「`operator>>`
/// 恰好 把 `\r` 當空白」這種隱性性質。
std::string normalizeLineEndings(const std::string& text) {
  std::string result;
  result.reserve(text.size());
  for (size_t i = 0; i < text.size(); ++i) {
    if (text[i] != '\r') {
      result += text[i];
      continue;
    }
    result += '\n';
    if (i + 1 < text.size() && text[i + 1] == '\n') ++i;
  }
  return result;
}

/// 將單一 UTF-8 碼點字串解回 char32_t（本測試所涉之鍵皆為單碼點）。
char32_t codepointToChar32(const std::string& codepoint) {
  if (codepoint.empty()) return 0;
  const unsigned char* bytes =
      reinterpret_cast<const unsigned char*>(codepoint.data());
  if ((bytes[0] & 0x80) == 0) return bytes[0];
  if ((bytes[0] & 0xE0) == 0xC0 && codepoint.size() >= 2)
    return static_cast<char32_t>(((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F));
  if ((bytes[0] & 0xF0) == 0xE0 && codepoint.size() >= 3)
    return static_cast<char32_t>(((bytes[0] & 0x0F) << 12) |
                                 ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F));
  if ((bytes[0] & 0xF8) == 0xF0 && codepoint.size() >= 4)
    return static_cast<char32_t>(((bytes[0] & 0x07) << 18) |
                                 ((bytes[1] & 0x3F) << 12) |
                                 ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F));
  return 0;
}

/// 鍵面字元之地面真值（靜態注音排列之按鍵域）：僅 ASCII 字母與數字。
///
/// 實查自素材檔之 1485 列 × 5 動態排列：其鍵面字元僅 `0-9` 與
/// `a-z`。反引號與空格
/// **不在其列**——兩者在素材檔內只作「無此鍵」之標記。此函式即靶之輸入域不變式。
bool isKeyCharacter(const std::string& codepoint) {
  if (codepoint.size() != 1) return false;
  const unsigned char byte = static_cast<unsigned char>(codepoint[0]);
  if (byte >= '0' && byte <= '9') return true;
  if (byte >= 'a' && byte <= 'z') return true;
  return byte >= 'A' && byte <= 'Z';
}

/// 整格是否皆為鍵面字元。
bool isAllKeyCharacters(const std::string& cell) {
  for (const auto& codepoint : splitByCodepoint(cell)) {
    if (!isKeyCharacter(codepoint)) return false;
  }
  return true;
}

/// 供診斷輸出之鍵面表現：非鍵面字元一律以 `U+XXXX` 呈現。
std::string shownKey(const std::string& key) {
  if (isKeyCharacter(key)) return key;
  std::string result;
  for (const auto& codepoint : splitByCodepoint(key)) {
    if (!result.empty()) result += " ";
    std::ostringstream stream;
    stream << "U+" << std::uppercase << std::hex
           << static_cast<unsigned>(codepointToChar32(codepoint));
    result += stream.str();
  }
  return result;
}

/// 單一按鍵之候選集（靜態注音排列之鍵面字元：數字 ＋ 小寫字母）。
///
/// **不得**再收反引號與空格：兩者非任何出貨排列之按鍵，見檔頭之說明。
std::vector<std::string> candidateKeys() {
  std::vector<std::string> result;
  for (const std::string& codepoint :
       splitByCodepoint("0123456789abcdefghijklmnopqrstuvwxyz")) {
    if (isKeyCharacter(codepoint)) result.push_back(codepoint);
  }
  return result;
}

/// 自素材檔就地解析 `testTable4DynamicLayouts` 之內容。
///
/// 僅解析一次，`rows` 與 `rawRowCount` 共用——**語料讀不到時必須大聲失敗**
/// （CI 實錄：Windows
/// 之語料整批讀不到，而當時之靶只在兩處下界斷言上失手）。
AutoChopCorpus autoChopCorpus() {
  AutoChopCorpus corpus;
  std::istringstream stream(
      normalizeLineEndings(TekkonTestData::testTable4DynamicLayouts));
  std::string line;
  while (std::getline(stream, line)) {
    std::istringstream lineStream(line);
    std::vector<std::string> tokens;
    std::string token;
    while (lineStream >> token) tokens.push_back(replaceUnderscores(token));
    // 表頭（`$READING Dachen26 …`）與空行非測資。
    if (tokens.empty() || tokens[0].rfind('$', 0) == 0) continue;
    ++corpus.rawRowCount;
    if (tokens.size() != 6) continue;
    // 校驗閘：任何單元格若含鍵面字元以外之字元即整列剔除。實查素材檔之此類單元格只有
    // 兩種：① 以反引號起始者（`` `NULL``、`` `vezf``…，標記「本排列無此鍵」）；
    // ② 尾端帶一空格者（`m `、`too `…，源自素材檔之 `__` ⇒ 空
    // cell）。**兩者皆為 「不適用」之標記，非按鍵。**
    const bool allCellsAreKeys = std::all_of(
        tokens.begin() + 1, tokens.end(),
        [](const std::string& cell) { return isAllKeyCharacters(cell); });
    if (!allCellsAreKeys) continue;
    AutoChopRow row;
    row.reading = tokens[0];
    row.cells.assign(tokens.begin() + 1, tokens.end());
    corpus.rows.push_back(row);
  }
  return corpus;
}

/// 逐碼點計長（Swift 側 `.count` 之對位；注音內容無合成字素）。
size_t codepointCount(const std::string& text) {
  return splitByCodepoint(text).size();
}

/// 由靜態排列之鍵表反推「注音符號（純量）→ 按鍵」。
///
/// **不**取用引擎之鍵表——改以純公開 API 逐鍵探測：把每個候選鍵餵進一枚空
/// `Composer`，看它填入哪個槽。靜態排列是一鍵一注音，故此探測即其鍵表之逆。
/// 同一符號多鍵時取候選序中最早者。
std::map<char32_t, std::string> staticKeyMap(MandarinParser parser) {
  std::map<char32_t, std::string> result;
  for (const auto& key : candidateKeys()) {
    Composer composer("", parser);
    composer.receiveKey(key);
    const std::vector<std::string> slots = {
        composer.consonant.value(), composer.semivowel.value(),
        composer.vowel.value(), composer.intonation.value()};
    std::string phonabet;
    size_t filledCount = 0;
    for (const auto& slot : slots) {
      if (slot.empty()) continue;
      ++filledCount;
      phonabet = slot;
    }
    if (filledCount != 1) continue;
    const char32_t scalar = codepointToChar32(phonabet);
    if (result.count(scalar) == 0) result[scalar] = key;
  }
  return result;
}

/// 將前若干筆診斷字串接成一行（避免測項輸出被數千筆誤切灌爆）。
std::string joinedSample(const std::vector<std::string>& sample,
                         size_t limit = 10) {
  std::string result;
  for (size_t i = 0; i < sample.size() && i < limit; ++i) {
    if (!result.empty()) result += " ";
    result += sample[i];
  }
  return result;
}

/// 以生產判準實際驅動一次輸入，回傳送入組字器之音節序列。
std::vector<std::string> typeWithProductionPredicate(const std::string& keys,
                                                     MandarinParser parser) {
  Composer composer("", parser);
  std::vector<std::string> committed;
  for (const auto& key : splitByCodepoint(keys)) {
    if (composer.shouldAutoChopPhonabets(codepointToChar32(key))) {
      const std::string reading = composer.phonabetKeyForQuery(true);
      if (!reading.empty()) {
        committed.push_back(reading);
        composer.clear();
      }
    }
    composer.receiveKey(key);
  }
  const std::string last = composer.phonabetKeyForQuery(true);
  if (!last.empty()) committed.push_back(last);
  return committed;
}

}  // namespace

@interface TekkonCCTests_PhonabetAutoChopPredicate : XCTestCase

@end

@implementation TekkonCCTests_PhonabetAutoChopPredicate

// =========== PHONABET AUTO-CHOP PREDICATE TESTS ===========

// MARK: ① 判準不得在單一音節內誤切

/// 合法單音節編碼之每一個中途前綴皆不得觸發切音節。
///
/// 覆蓋 **11 個排列**：5 個動態排列用語料之 1485
/// 列編碼；6 個靜態排列用其鍵表反推全部前綴。
- (void)test_PhonabetAutoChopPredicate_NeverFiresWithinASyllable {
  const AutoChopCorpus corpus = autoChopCorpus();
  XCTAssertFalse(corpus.rows.empty(), @"語料解析失敗——素材檔格式已變：\n%@",
                 nsString(corpus.report()));
  std::vector<std::string> offenders;
  long long steps = 0;

  for (size_t layoutIndex = 0; layoutIndex < dynamicLayouts().size();
       ++layoutIndex) {
    const AutoChopLayout& layout = dynamicLayouts()[layoutIndex];
    for (const AutoChopRow& row : corpus.rows) {
      const std::string& cell = row.cells[layoutIndex];
      if (!isAllKeyCharacters(cell)) continue;
      Composer composer("", layout.parser);
      int step = 0;
      for (const auto& key : splitByCodepoint(cell)) {
        ++steps;
        if (composer.shouldAutoChopPhonabets(codepointToChar32(key))) {
          offenders.push_back(layout.name + "/" + row.reading + "/拍" +
                              std::to_string(step) + "/鍵`" + shownKey(key) +
                              "`");
        }
        composer.receiveKey(key);
        ++step;
      }
    }
  }

  // 前綴集由 `readings()` 就地推導——索引本身刻意不暴露
  // `allPrefixes`（只答「是否為前綴」一問），故本靶自行展開、再逐條以
  // `isPrefix` 交叉驗證。
  const SyllableIndex& index = SyllableIndex::shared(ofDachen);
  std::set<std::string> allPrefixes;
  for (const auto& reading : index.readings()) {
    std::string accumulated;
    for (const auto& codepoint : splitByCodepoint(reading)) {
      accumulated += codepoint;
      allPrefixes.insert(accumulated);
    }
  }
  for (const AutoChopLayout& layout : staticLayouts()) {
    const std::map<char32_t, std::string> keyMap = staticKeyMap(layout.parser);
    // 靶之輸入域不變式：反推所得之按鍵一律須為鍵面字元。**此行即迴歸釘**——先前之
    // 候選鍵含反引號與空格，反推遂把它們登記成某注音符號之按鍵，而由合法讀音之前綴
    // 生成出**不可鍵入**之鍵序（CI 實錄：Linux 誤切 7366 次，全數為該等鍵）。
    for (const auto& pair : keyMap) {
      XCTAssertTrue(isKeyCharacter(pair.second),
                    @"%@ 之反推鍵表含非鍵面字元：%@", nsString(layout.name),
                    nsString(shownKey(pair.second)));
    }
    for (const auto& reading : allPrefixes) {
      // 就地推導之前綴集須與索引自身之判準一致（索引為排列中立）。
      XCTAssertTrue(index.isPrefix(reading), @"%@", nsString(reading));
      std::vector<std::string> keys;
      bool keysComplete = true;
      for (const auto& codepoint : splitByCodepoint(reading)) {
        auto iter = keyMap.find(codepointToChar32(codepoint));
        if (iter == keyMap.end()) {
          keysComplete = false;
          break;
        }
        keys.push_back(iter->second);
      }
      if (!keysComplete) continue;
      Composer composer("", layout.parser);
      int step = 0;
      for (const auto& key : keys) {
        ++steps;
        if (composer.shouldAutoChopPhonabets(codepointToChar32(key))) {
          offenders.push_back(layout.name + "/" + reading + "/拍" +
                              std::to_string(step) + "/鍵`" + shownKey(key) +
                              "`");
        }
        composer.receiveKey(key);
        ++step;
      }
    }
  }

  // 下界改以**結構**表達，不再釘死魔數：語料須幾近全數解析成功、且受檢步數須成規模。
  XCTAssertGreaterThan(corpus.rawRowCount, 1000, @"素材檔之資料列僅 %d：\n%@",
                       corpus.rawRowCount, nsString(corpus.report()));
  // 實測剔除率 38/1485 ＝ 2.56%（全為上揭兩種「不適用」標記）；門檻取 95%
  // 以為餘裕， 意在攔住「整批讀不到」與「大規模解析失敗」，而非逐列計較。
  XCTAssertGreaterThanOrEqual(
      static_cast<int>(corpus.rows.size()) * 100, corpus.rawRowCount * 95,
      @"語料解析損失過大：%lu / %d", (unsigned long)corpus.rows.size(),
      corpus.rawRowCount);
  XCTAssertGreaterThan(steps, 20000LL, @"受檢步數僅 %lld：\n%@", steps,
                       nsString(corpus.report()));
  XCTAssertTrue(offenders.empty(), @"誤切 %lu 次：%@",
                (unsigned long)offenders.size(),
                nsString(joinedSample(offenders)));
}

// MARK: ② 判準須在音節交界處切分

/// 音節交界處必須切。地面真相：A ＝ 一完整合法讀音；本鍵若**不能**把 A
/// 延伸成更長之合法前綴，則 A 須先固化 ⇒ 必切。
- (void)test_PhonabetAutoChopPredicate_FiresAtJunctions {
  const AutoChopCorpus corpus = autoChopCorpus();
  const SyllableIndex& index = SyllableIndex::shared(ofDachen);
  long long checked = 0;
  long long missed = 0;
  std::vector<std::string> sample;

  for (size_t layoutIndex = 0; layoutIndex < dynamicLayouts().size();
       ++layoutIndex) {
    const AutoChopLayout& layout = dynamicLayouts()[layoutIndex];
    std::set<std::string> keys;
    std::vector<Composer> states;
    for (const AutoChopRow& row : corpus.rows) {
      const std::string& cell = row.cells[layoutIndex];
      if (!isAllKeyCharacters(cell)) continue;
      std::vector<std::string> codepoints = splitByCodepoint(cell);
      for (const auto& key : codepoints) keys.insert(key);
      Composer composer("", layout.parser);
      for (const auto& key : codepoints) composer.receiveKey(key);
      states.push_back(composer);
    }
    for (Composer& composer : states) {
      const std::string preContent = composer.getComposition();
      for (const auto& key : keys) {
        Composer probe = composer;
        probe.receiveKey(key);
        const std::string postContent = probe.getComposition();
        // 於**當前狀態**下寫入聲調槽者（聲調鍵／空格）由既有管線固化，不屬本案（照原靶之守衛）。
        if (probe.intonation.value() != composer.intonation.value()) continue;
        const bool greedy =
            index.isPrefix(postContent) &&
            codepointCount(postContent) > codepointCount(preContent);
        if (greedy) continue;
        ++checked;
        if (!composer.shouldAutoChopPhonabets(codepointToChar32(key))) {
          ++missed;
          if (sample.size() < 5)
            sample.push_back(layout.name + "/" + preContent + "+`" +
                             shownKey(key) + "`");
        }
      }
    }
  }

  // 同上：交界數之下界為結構量（0 即語料未載入）；「不得漏切」由 `missed`
  // 承擔。
  XCTAssertGreaterThan(checked, 0LL, @"受檢交界僅 %lld：\n%@", checked,
                       nsString(corpus.report()));
  // 術前驗證之實測為 2.19%；此處以 5% 為上限——殘餘之成因（與 `qquu`
  // 之逐槽覆寫在局部可觀測量上同構）已證不可由局部判準分離，屬**已知界線**。
  const double rate = static_cast<double>(missed) * 100.0 /
                      static_cast<double>(checked > 0 ? checked : 1);
  XCTAssertLessThan(rate, 5.0, @"漏切率 %f%%（%lld/%lld）；樣本：%@", rate,
                    missed, checked, nsString(joinedSample(sample, 5)));
}

// MARK: ③ 單聲母縮寫

/// 單聲母縮寫：`ess`（大千：ㄍ＝`e`、ㄋ＝`s`）須得三顆鍵。
- (void)
    test_PhonabetAutoChopPredicate_SinglePhonabetAbbreviationIsCommittedAsThreeKeys {
  XCTAssertTrue(typeWithProductionPredicate("ess", ofDachen) ==
                (std::vector<std::string>{"ㄍ", "ㄋ", "ㄋ"}));
  XCTAssertTrue(typeWithProductionPredicate("ss", ofDachen) ==
                (std::vector<std::string>{"ㄋ", "ㄋ"}));
  // 反向護欄：完整讀音不得被切碎。
  XCTAssertTrue(typeWithProductionPredicate("1u0", ofDachen) ==
                (std::vector<std::string>{"ㄅㄧㄢ"}));
}

// MARK: ④ 動態排列之逐槽覆寫

/// 動態排列之逐槽覆寫：大千26 `qquu`＝ㄅㄚ 之四拍不得被切斷。
- (void)
    test_PhonabetAutoChopPredicate_Dachen26SlotOverwriteSurvivesThePredicate {
  Composer composer("", ofDachen26);
  std::vector<bool> verdicts;
  for (const auto& key : splitByCodepoint("qquu")) {
    verdicts.push_back(
        composer.shouldAutoChopPhonabets(codepointToChar32(key)));
    composer.receiveKey(key);
  }
  std::string verdictsText;
  for (const bool verdict : verdicts) verdictsText += verdict ? "1" : "0";
  XCTAssertTrue(verdicts == (std::vector<bool>{false, false, false, false}),
                @"實得：%@", nsString(verdictsText));
  XCTAssertTrue(composer.getComposition() == "ㄅㄚ");
}

@end
