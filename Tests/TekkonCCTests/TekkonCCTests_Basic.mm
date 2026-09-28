// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#import "Tekkon.hh"

using namespace Tekkon;

@interface TekkonCCTests_Basic : XCTestCase

@end

@implementation TekkonCCTests_Basic

- (void)test_Basic_InitializingPhonabet {
  Phonabet thePhonabetNull = Phonabet("0");
  Phonabet thePhonabetA = Phonabet("ㄉ");
  Phonabet thePhonabetB = Phonabet("ㄧ");
  Phonabet thePhonabetC = Phonabet("ㄠ");
  Phonabet thePhonabetD = Phonabet("ˇ");
  XCTAssertEqual(thePhonabetNull.type, PhoneType::null);
  XCTAssertEqual(thePhonabetA.type, PhoneType::consonant);
  XCTAssertEqual(thePhonabetB.type, PhoneType::semivowel);
  XCTAssertEqual(thePhonabetC.type, PhoneType::vowel);
  XCTAssertEqual(thePhonabetD.type, PhoneType::intonation);
}

- (void)test_Basic_IsValidKeyWithKeys {
  bool result;
  Tekkon::Composer composer = Composer("", ofDachen);

  /// Testing Failed Key
  result = composer.inputValidityCheck(0x0024);
  XCTAssertFalse(result);

  // Testing Correct Qwerty Dachen Key
  composer.ensureParser(ofDachen);
  result = composer.inputValidityCheck(0x002F);
  XCTAssertTrue(result);

  // Testing Correct ETen26 Key
  composer.ensureParser(ofETen26);
  result = composer.inputValidityCheck(0x0062);
  XCTAssertTrue(result);

  // Testing Correct Hanyu-Pinyin Key
  composer.ensureParser(ofHanyuPinyin);
  result = composer.inputValidityCheck(0x0062);
  XCTAssertTrue(result);
}

// =========== COMPOSER POKAYOKE TESTS ===========

- (void)test_Basic_PhonabetCombinationCorrection {
  Composer composer = Composer("", ofDachen, true);

  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄧ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄓˋ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄐㄩˋ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄧ");
  composer.receiveKeyFromPhonabet("ㄢ");
  XCTAssertEqual(composer.value(), "ㄓㄢ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ㄢ");
  XCTAssertEqual(composer.value(), "ㄐㄩㄢ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄧ");
  composer.receiveKeyFromPhonabet("ㄢ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄓㄢˋ");

  composer.clear();
  composer.receiveKeyFromPhonabet("ㄓ");
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ㄢ");
  composer.receiveKeyFromPhonabet("ˋ");
  XCTAssertEqual(composer.value(), "ㄐㄩㄢˋ");
}

- (void)test_Basic_SemivowelNormalizationWithEncounteredVowels {
  // 測試「ㄩ」遇到特定韻母時應即時轉為「ㄨ」的邏輯。
  Composer composer = Composer("", ofDachen, true);
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ㄛ");
  XCTAssertEqual(composer.value(), "ㄨㄛ");

  // 測試聲母存在時依然維持正確輸出。
  composer.clear();
  composer.receiveKeyFromPhonabet("ㄅ");
  composer.receiveKeyFromPhonabet("ㄩ");
  composer.receiveKeyFromPhonabet("ㄛ");
  XCTAssertEqual(composer.getComposition(), "ㄅㄛ");
}

- (void)test_Basic_PronounceableQueryKeyGate {
  // 測試 pronounceableOnly 旗標僅允許可唸組合的結果通過。
  Composer composer = Composer("", ofDachen, false);
  composer.receiveKeyFromPhonabet("ˊ");
  XCTAssertFalse(composer.isPronounceable());
  XCTAssertEqual(composer.phonabetKeyForQuery(true), "");
  XCTAssertEqual(composer.phonabetKeyForQuery(false), "ˊ");
}

- (void)test_Basic_PinyinTrieBranchInsertKeepsExistingBranches {
  // 測試 PinyinTrie 在同一節點新增多個分支時，既有讀音不會被覆蓋。
  PinyinTrie trie = PinyinTrie(ofDachen);
  trie.insert("li", "ㄌㄧ");
  trie.insert("lin", "ㄌㄧㄣ");
  trie.insert("liu", "ㄌㄧㄡ");

  std::vector<std::string> fetched = trie.search("li");
  auto contains = [&fetched](const std::string& needle) {
    for (const std::string& target : fetched) {
      if (target == needle) return true;
    }
    return false;
  };
  XCTAssertTrue(contains("ㄌㄧ"));
  XCTAssertTrue(contains("ㄌㄧㄣ"));
  XCTAssertTrue(contains("ㄌㄧㄡ"));
}
// =========== PHONABET TYPINNG HANDLING TESTS (BASIC) ===========

- (void)test_Basic_PhonabetKeyReceivingAndCompositions {
  Composer composer = Composer("", ofDachen);
  bool toneMarkerIndicator;

  // Test Key Receiving;
  composer.receiveKey(0x0032);  // 2, ㄉ
  composer.receiveKey("j");     // ㄨ
  composer.receiveKey("u");     // ㄧ
  composer.receiveKey("l");     // ㄠ

  // Testing missing tone markers;
  toneMarkerIndicator = composer.hasIntonation();
  XCTAssertTrue(!toneMarkerIndicator);

  composer.receiveKey("3");  // 上聲
  XCTAssertEqual(composer.value(), "ㄉㄧㄠˇ");
  composer.doBackSpace();
  composer.receiveKey(" ");  // 陰平
  XCTAssertEqual(composer.value(),
                 "ㄉㄧㄠ ");  // 這裡回傳的結果的陰平是空格

  // Test Getting Displayed Composition
  XCTAssertEqual(composer.getComposition(), "ㄉㄧㄠ");
  XCTAssertEqual(composer.getComposition(true, false),
                 "diao1");  // 中階測試項目
  XCTAssertEqual(composer.getComposition(true, true),
                 "diāo");  // 中階測試項目
  XCTAssertEqual(composer.getInlineCompositionForDisplay(true),
                 "diao1");  // 中階測試項目

  // Test Tone 5
  composer.receiveKey("7");  // 輕聲
  XCTAssertEqual(composer.getComposition(), "ㄉㄧㄠ˙");
  XCTAssertEqual(composer.getComposition(false, true),
                 "˙ㄉㄧㄠ");  // 中階測試項目

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

  // Testing auto phonabet combination fixing process.
  composer.phonabetCombinationCorrectionEnabled = true;

  // Testing exceptions of handling "ㄅㄨㄛ ㄆㄨㄛ ㄇㄨㄛ ㄈㄨㄛ"
  composer.clear();
  composer.receiveKey("1");
  composer.receiveKey("j");
  composer.receiveKey("i");
  XCTAssertEqual(composer.getComposition(), "ㄅㄛ");
  composer.receiveKey("q");
  XCTAssertEqual(composer.getComposition(), "ㄆㄛ");
  composer.receiveKey("a");
  XCTAssertEqual(composer.getComposition(), "ㄇㄛ");
  composer.receiveKey("z");
  XCTAssertEqual(composer.getComposition(), "ㄈㄛ");

  // Testing exceptions of handling "ㄅㄨㄥ ㄆㄨㄥ ㄇㄨㄥ ㄈㄨㄥ"
  composer.clear();
  composer.receiveKey("1");
  composer.receiveKey("j");
  composer.receiveKey("/");
  XCTAssertEqual(composer.getComposition(), "ㄅㄥ");
  composer.receiveKey("q");
  XCTAssertEqual(composer.getComposition(), "ㄆㄥ");
  composer.receiveKey("a");
  XCTAssertEqual(composer.getComposition(), "ㄇㄥ");
  composer.receiveKey("z");
  XCTAssertEqual(composer.getComposition(), "ㄈㄥ");

  // Testing exceptions of handling "ㄋㄨㄟ ㄌㄨㄟ"
  composer.clear();
  composer.receiveKey("s");
  composer.receiveKey("j");
  composer.receiveKey("o");
  XCTAssertEqual(composer.getComposition(), "ㄋㄟ");
  composer.receiveKey("x");
  XCTAssertEqual(composer.getComposition(), "ㄌㄟ");

  // Testing exceptions of handling "ㄧㄜ ㄩㄜ"
  composer.clear();
  composer.receiveKey("s");
  composer.receiveKey("k");
  composer.receiveKey("u");
  XCTAssertEqual(composer.getComposition(), "ㄋㄧㄝ");
  composer.receiveKey("s");
  composer.receiveKey("m");
  composer.receiveKey("k");
  XCTAssertEqual(composer.getComposition(), "ㄋㄩㄝ");
  composer.receiveKey("s");
  composer.receiveKey("u");
  composer.receiveKey("k");
  XCTAssertEqual(composer.getComposition(), "ㄋㄧㄝ");

  // Testing exceptions of handling "ㄨㄜ ㄨㄝ"
  composer.clear();
  composer.receiveKey("j");
  composer.receiveKey("k");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");
  composer.clear();
  composer.receiveKey("j");
  composer.receiveKey(",");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");
  composer.clear();
  composer.receiveKey(",");
  composer.receiveKey("j");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");
  composer.clear();
  composer.receiveKey("k");
  composer.receiveKey("j");
  XCTAssertEqual(composer.getComposition(), "ㄩㄝ");

  // Testing tool functions
  XCTAssertEqual(Tekkon::restoreToneOneInPhona("ㄉㄧㄠ"), "ㄉㄧㄠ1");
  XCTAssertEqual(Tekkon::cnvPhonaToTextbookStyle("ㄓㄜ˙"), "˙ㄓㄜ");
  XCTAssertEqual(Tekkon::cnvPhonaToHanyuPinyin("ㄍㄢˋ"), "gan4");
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToTextBookStyle("起(qi3)居(ju1)"),
                 "起(qǐ)居(jū)");
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToPhona("bian4"), "ㄅㄧㄢˋ");
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToPhona("bian4-le5-tian1"),
                 "ㄅㄧㄢˋ-ㄌㄜ˙-ㄊㄧㄢ");
  // 測試這種情形：「如果傳入的字串不包含任何半形英數內容的話，那麼應該直接將傳入的字串原樣返回」。
  XCTAssertEqual(Tekkon::cnvHanyuPinyinToPhona("ㄅㄧㄢˋ-˙ㄌㄜ-ㄊㄧㄢ"),
                 "ㄅㄧㄢˋ-˙ㄌㄜ-ㄊㄧㄢ");
}

- (void)test_Basic_MandarinParser {
  // 排列家族：拼音、動態注音、靜態注音三者互斥，其聯集為全體。
  XCTAssertEqual(allPinyinCases().size(), 6UL);
  XCTAssertEqual(allDynamicZhuyinCases().size(), 5UL);
  XCTAssertEqual(allStaticZhuyinCases().size(), 6UL);
  XCTAssertEqual(allPinyinCases().size() + allDynamicZhuyinCases().size() +
                     allStaticZhuyinCases().size(),
                 allCases().size());
  // 診斷用名稱：全體互異，且不得退化為數值字串。
  std::set<std::string> nameTags;
  for (MandarinParser parser : allCases()) {
    const std::string tag = nameTag(parser);
    XCTAssertFalse(tag.empty());
    XCTAssertTrue(tag != std::to_string(static_cast<int>(parser)));
    nameTags.insert(tag);
  }
  XCTAssertEqual(nameTags.size(), allCases().size());
  XCTAssertTrue(nameTag(ofDachen) == "Dachen");
  XCTAssertTrue(nameTag(ofHanyuPinyin) == "HanyuPinyin");

  const std::vector<MandarinParser> staticCases = allStaticZhuyinCases();
  for (MandarinParser parser : allCases()) {
    XCTAssertEqual(isPinyin(parser), parser >= 100);
    if (isPinyin(parser)) XCTAssertFalse(isDynamic(parser));
    const bool isStatic =
        std::count(staticCases.begin(), staticCases.end(), parser) == 1;
    XCTAssertTrue(isPinyin(parser) || isDynamic(parser) || isStatic);
  }

  // 「拼音 -> 注音」對照表僅拼音排列有之。
  XCTAssertTrue(mapZhuyinPinyin(ofDachen) == nullptr);
  XCTAssertTrue(mapZhuyinPinyin(ofHanyuPinyin) != nullptr);
  XCTAssertEqual(mapZhuyinPinyin(ofHanyuPinyin)->size(), 426UL);

  // 全部可能讀音＝讀音詞幹 ＋ 其與各聲調之組合；聲調之表記隨排列家族而異。
  const std::set<std::string> hanyuReadings =
      allPossibleReadings(ofHanyuPinyin);
  XCTAssertEqual(hanyuReadings.count("bian"), 1UL);
  XCTAssertEqual(hanyuReadings.count("bian1"), 1UL);
  XCTAssertEqual(hanyuReadings.count("bian5"), 1UL);
  XCTAssertEqual(hanyuReadings.count("bianˊ"), 0UL);
  const std::set<std::string> dachenReadings = allPossibleReadings(ofDachen);
  XCTAssertEqual(dachenReadings.count("ㄅㄧㄢ"), 1UL);
  XCTAssertEqual(dachenReadings.count("ㄅㄧㄢˋ"), 1UL);
  XCTAssertEqual(dachenReadings.count("ㄅㄧㄢ "), 1UL);

  // 逐排列送鍵（含非按鍵之輸入），再清空；每一條排列分支皆須走過且不得崩潰。
  Composer composer("", ofDachen);
  XCTAssertTrue(composer.isEmpty());
  XCTAssertEqual(composer.count(true), 0);
  XCTAssertEqual(composer.count(false), 0);
  for (MandarinParser parser : allCases()) {
    composer.ensureParser(parser);
    composer.receiveKey("q");
    composer.receiveKey("幹");
    composer.receiveKey("3");
    composer.receiveKey("1");
    composer.clear();
    XCTAssertTrue(composer.isEmpty());
  }
}

- (void)test_Basic_ChoppingRawComplex {
  PinyinTrie trieZhuyin(ofDachen);
  PinyinTrie triePinyin(ofHanyuPinyin);
  {
    // 注音排列：以讀音表逐段做最長前綴比對。
    XCTAssertTrue(trieZhuyin.chop("ㄅㄩㄝㄓㄨㄑㄕㄢㄌㄧㄌㄧㄤ") ==
                  (std::vector<std::string>{"ㄅ", "ㄩㄝ", "ㄓㄨ", "ㄑ", "ㄕㄢ",
                                            "ㄌㄧ", "ㄌㄧㄤ"}));
    // 漢語拼音：沿 trie 貪婪下探。
    XCTAssertTrue(
        triePinyin.chop("byuezqsll") ==
        (std::vector<std::string>{"b", "yue", "z", "q", "s", "l", "l"}));
    XCTAssertTrue(trieZhuyin.chop("ㄕㄐㄧㄉㄓ") ==
                  (std::vector<std::string>{"ㄕ", "ㄐㄧ", "ㄉ", "ㄓ"}));
  }
  {
    const std::vector<std::string> choppedPinyin = triePinyin.chop("yod");
    XCTAssertTrue(choppedPinyin == (std::vector<std::string>{"yo", "d"}));
    // 切分結果再展開為注音：單一拼音切片可以對應多個注音。
    const std::vector<std::string> deducted =
        triePinyin.deductChoppedPinyinToZhuyin(choppedPinyin);
    XCTAssertFalse(deducted.empty());
    XCTAssertEqual(deducted.front(), "ㄧㄛ&ㄧㄡ&ㄩㄥ");
  }
}

- (void)test_Basic_PinyinTrieConvertingPinyinChopsToZhuyin {
  // 漢語拼音。
  {
    PinyinTrie trie(ofHanyuPinyin);
    const std::vector<std::string> choppedPinyin = {"b", "yue", "z", "q",
                                                    "s", "l",   "l"};
    const std::vector<std::string> expected = {"ㄅ",    "ㄩㄝ", "ㄓ&ㄗ", "ㄑ",
                                               "ㄕ&ㄙ", "ㄌ",   "ㄌ"};
    XCTAssertTrue(trie.deductChoppedPinyinToZhuyin(choppedPinyin) == expected);
  }
  // 國音二式。
  {
    PinyinTrie trie(ofSecondaryPinyin);
    const std::vector<std::string> choppedPinyin = {"ch", "f", "h", "s"};
    const std::vector<std::string> expected = {"ㄑ&ㄔ", "ㄈ", "ㄏ", "ㄒ&ㄕ&ㄙ"};
    XCTAssertTrue(trie.deductChoppedPinyinToZhuyin(choppedPinyin) == expected);
  }
  // 耶魯拼音。
  {
    PinyinTrie trie(ofYalePinyin);
    const std::vector<std::string> choppedPinyin = {"ch", "f", "h", "s"};
    const std::vector<std::string> expected = {"ㄑ&ㄔ", "ㄈ", "ㄏ", "ㄒ&ㄕ&ㄙ"};
    XCTAssertTrue(trie.deductChoppedPinyinToZhuyin(choppedPinyin) == expected);
  }
  // 華羅拼音。
  {
    PinyinTrie trie(ofHualuoPinyin);
    const std::vector<std::string> choppedPinyin = {"ch", "f", "h", "s"};
    const std::vector<std::string> expected = {"ㄑ&ㄔ", "ㄈ", "ㄏ", "ㄒ&ㄕ&ㄙ"};
    XCTAssertTrue(trie.deductChoppedPinyinToZhuyin(choppedPinyin) == expected);
  }
  // 通用拼音。
  {
    PinyinTrie trie(ofUniversalPinyin);
    const std::vector<std::string> choppedPinyin = {"ch", "f", "h", "s"};
    const std::vector<std::string> expected = {"ㄔ", "ㄈ", "ㄏ", "ㄒ&ㄕ&ㄙ"};
    XCTAssertTrue(trie.deductChoppedPinyinToZhuyin(choppedPinyin) == expected);
  }
  // 韋氏拼音。
  {
    PinyinTrie trie(ofWadeGilesPinyin);
    const std::vector<std::string> choppedPinyin = {"ch", "f", "h", "s"};
    const std::vector<std::string> expected = {"ㄐ&ㄑ&ㄓ&ㄔ", "ㄈ", "ㄏ&ㄒ",
                                               "ㄕ&ㄙ"};
    XCTAssertTrue(trie.deductChoppedPinyinToZhuyin(choppedPinyin) == expected);
  }
}

@end
