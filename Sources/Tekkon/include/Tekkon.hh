// (c) 2022 and onwards The vChewing Project (MIT-NTL License).
// ====================
// This code is released under the MIT license (SPDX-License-Identifier: MIT)
// ... with NTL restriction stating that:
// No trademark license is granted to use the trade names, trademarks, service
// marks, or product names of Contributor, except as required to fulfill notice
// requirements defined in MIT License.

// ADVICE: Save as UTF8 without BOM signature!!!

/// 鐵恨引擎是 OVMandarin 的繼任者，支援酷音大千26排列、以及更多種類的拼音輸入。
/// 因為 Cpp 講究事物相生順序，所以很多龐大的陣列常數不能像 Swift 原版的鐵恨引擎
/// 那樣放在檔案末尾，就只能放在檔案開頭了。另外，Cpp 的 select case 語句是可以
/// 將 std::string 拿來 switch 的，但必須做雜湊化處理（hashify）使其整數化。

// This module conforms to Cpp17 standard.
// Clang-14 or higher is recommended for compilation.
// If using Swift Package Manager to compile, use Swift 5.9 or newer.

#ifndef TEKKON_HH_
#define TEKKON_HH_

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// The namespace for this package.
namespace Tekkon {

// MARK: - 幾個工具函式（語法糖），讓 Cpp 更好用。

inline static std::string charToString(char theChar) {
  std::string result(1, theChar);
  return result;
}

inline static void replaceOccurrences(std::string& data, std::string toSearch,
                                      std::string replaceStr) {
  size_t position = data.find(toSearch);
  while (position != std::string::npos) {
    data.replace(position, toSearch.size(), replaceStr);
    position = data.find(toSearch, position + replaceStr.size());
  }
}

template <typename Type>
static bool contains(std::vector<Type> theVector, const Type& theElement) {
  if (std::find(theVector.begin(), theVector.end(), theElement) !=
      theVector.end())
    return true;
  return false;
}

constexpr unsigned int hashify(const char* str, int h = 0) {
  return !str[h] ? 5381 : (hashify(str, h + 1) * 33) ^ str[h];
}

template <typename T>
std::vector<T> operator+(std::vector<T> const& x, std::vector<T> const& y) {
  std::vector<T> vec;
  vec.reserve(x.size() + y.size());
  vec.insert(vec.end(), x.begin(), x.end());
  vec.insert(vec.end(), y.begin(), y.end());
  return vec;
}

template <typename T>
std::vector<T>& operator+=(std::vector<T>& x, std::vector<T>& y) {
  x.reserve(x.size() + y.size());
  x.insert(x.end(), y.begin(), y.end());
  return x;
}

// The following function is taken from boost internals.
// License: https://www.boost.org/users/license.html
inline static unsigned utf8ByteCount(unsigned theChar) {
  // if the most significant bit with a zero in it is in position
  // 8-N then there are N bytes in this UTF-8 sequence:
  unsigned mask = 0x80u;
  unsigned result = 0;
  while (theChar & mask) {
    ++result;
    mask >>= 1;
  }
  return (result == 0) ? 1 : ((result > 4) ? 4 : result);
}

// The following function is taken from boost internals.
// License: https://www.boost.org/users/license.html
inline static std::vector<std::string> splitByCodepoint(std::string input) {
  std::vector<std::string> arrReturned;
  auto netaIterated = input.cbegin();
  while (netaIterated != input.cend()) {
    unsigned count = utf8ByteCount(*netaIterated);
    arrReturned.emplace_back(std::string{netaIterated, netaIterated + count});
    netaIterated += count;
  }
  return arrReturned;
}

/// 檢查第二個字串參數是否被第一個字串參數所包含。
/// @param able 第一個字串參數。
/// @param baker 第二個字串參數。
inline static bool stringInclusion(std::string able, std::string baker) {
  return (baker.empty()) ? able.empty()
                         : (able.find(baker, 0) != std::string::npos);
}

// MARK: - Static Constants and Basic Enums

/// 定義注音符號的種類
enum PhoneType : int {
  null = 0,        // 假
  consonant = 1,   // 聲
  semivowel = 2,   // 介
  vowel = 3,       // 韻
  intonation = 4,  // 調
};

/// 定義注音排列的類型
enum MandarinParser : int {
  ofDachen = 0,
  ofDachen26 = 1,
  ofETen = 2,
  ofETen26 = 3,
  ofHsu = 4,
  ofIBM = 5,
  ofMiTAC = 6,
  ofSeigyou = 7,
  ofFakeSeigyou = 8,
  ofStarlight = 9,
  ofAlvinLiu = 10,
  ofHanyuPinyin = 100,
  ofSecondaryPinyin = 101,
  ofYalePinyin = 102,
  ofHualuoPinyin = 103,
  ofUniversalPinyin = 104,
  ofWadeGilesPinyin = 105
};

/// 引擎僅接受這些記號作為聲母
inline static std::vector<std::string> allowedConsonants = {
    "ㄅ", "ㄆ", "ㄇ", "ㄈ", "ㄉ", "ㄊ", "ㄋ", "ㄌ", "ㄍ", "ㄎ", "ㄏ",
    "ㄐ", "ㄑ", "ㄒ", "ㄓ", "ㄔ", "ㄕ", "ㄖ", "ㄗ", "ㄘ", "ㄙ"};

/// 引擎僅接受這些記號作為介母
inline static std::vector<std::string> allowedSemivowels = {"ㄧ", "ㄨ", "ㄩ"};

/// 引擎僅接受這些記號作為韻母
inline static std::vector<std::string> allowedVowels = {
    "ㄚ", "ㄛ", "ㄜ", "ㄝ", "ㄞ", "ㄟ", "ㄠ",
    "ㄡ", "ㄢ", "ㄣ", "ㄤ", "ㄥ", "ㄦ"};

/// 引擎僅接受這些記號作為聲調
inline static std::vector<std::string> allowedIntonations = {" ", "ˊ", "ˇ", "ˋ",
                                                             "˙"};

/// 引擎僅接受這些記號作為注音（聲介韻調四個集合加起來）
inline static std::vector<std::string> allowedPhonabets =
    allowedConsonants + allowedSemivowels + allowedVowels + allowedIntonations;

/// 原始轉換對照表資料貯存專用佇列（數字標調格式）
inline static std::vector<std::vector<std::string>> arrPhonaToHanyuPinyin =
    {  // 排序很重要。先處理最長的，再處理短的。不然會出亂子。
        {" ", "1"},           {"ˊ", "2"},           {"ˇ", "3"},
        {"ˋ", "4"},           {"˙", "5"},

        {"ㄅㄧㄝ", "bie"},    {"ㄅㄧㄠ", "biao"},   {"ㄅㄧㄢ", "bian"},
        {"ㄅㄧㄣ", "bin"},    {"ㄅㄧㄥ", "bing"},   {"ㄆㄧㄚ", "pia"},
        {"ㄆㄧㄝ", "pie"},    {"ㄆㄧㄠ", "piao"},   {"ㄆㄧㄢ", "pian"},
        {"ㄆㄧㄣ", "pin"},    {"ㄆㄧㄥ", "ping"},   {"ㄇㄧㄝ", "mie"},
        {"ㄇㄧㄠ", "miao"},   {"ㄇㄧㄡ", "miu"},    {"ㄇㄧㄢ", "mian"},
        {"ㄇㄧㄣ", "min"},    {"ㄇㄧㄥ", "ming"},   {"ㄈㄧㄠ", "fiao"},
        {"ㄈㄨㄥ", "fong"},   {"ㄉㄧㄚ", "dia"},    {"ㄉㄧㄝ", "die"},
        {"ㄉㄧㄠ", "diao"},   {"ㄉㄧㄡ", "diu"},    {"ㄉㄧㄢ", "dian"},
        {"ㄉㄧㄥ", "ding"},   {"ㄉㄨㄛ", "duo"},    {"ㄉㄨㄟ", "dui"},
        {"ㄉㄨㄢ", "duan"},   {"ㄉㄨㄣ", "dun"},    {"ㄉㄨㄥ", "dong"},
        {"ㄊㄧㄝ", "tie"},    {"ㄊㄧㄠ", "tiao"},   {"ㄊㄧㄢ", "tian"},
        {"ㄊㄧㄥ", "ting"},   {"ㄊㄨㄛ", "tuo"},    {"ㄊㄨㄟ", "tui"},
        {"ㄊㄨㄢ", "tuan"},   {"ㄊㄨㄣ", "tun"},    {"ㄊㄨㄥ", "tong"},
        {"ㄋㄧㄝ", "nie"},    {"ㄋㄧㄠ", "niao"},   {"ㄋㄧㄡ", "niu"},
        {"ㄋㄧㄢ", "nian"},   {"ㄋㄧㄣ", "nin"},    {"ㄋㄧㄤ", "niang"},
        {"ㄋㄧㄥ", "ning"},   {"ㄋㄨㄛ", "nuo"},    {"ㄋㄨㄟ", "nui"},
        {"ㄋㄨㄢ", "nuan"},   {"ㄋㄨㄣ", "nun"},    {"ㄋㄨㄥ", "nong"},
        {"ㄋㄩㄝ", "nve"},    {"ㄌㄧㄚ", "lia"},    {"ㄌㄧㄝ", "lie"},
        {"ㄌㄧㄠ", "liao"},   {"ㄌㄧㄡ", "liu"},    {"ㄌㄧㄢ", "lian"},
        {"ㄌㄧㄣ", "lin"},    {"ㄌㄧㄤ", "liang"},  {"ㄌㄧㄥ", "ling"},
        {"ㄌㄨㄛ", "luo"},    {"ㄌㄨㄢ", "luan"},   {"ㄌㄨㄣ", "lun"},
        {"ㄌㄨㄥ", "long"},   {"ㄌㄩㄝ", "lve"},    {"ㄌㄩㄢ", "lvan"},
        {"ㄍㄧㄠ", "giao"},   {"ㄍㄧㄣ", "gin"},    {"ㄍㄨㄚ", "gua"},
        {"ㄍㄨㄛ", "guo"},    {"ㄍㄨㄜ", "gue"},    {"ㄍㄨㄞ", "guai"},
        {"ㄍㄨㄟ", "gui"},    {"ㄍㄨㄢ", "guan"},   {"ㄍㄨㄣ", "gun"},
        {"ㄍㄨㄤ", "guang"},  {"ㄍㄨㄥ", "gong"},   {"ㄎㄧㄡ", "kiu"},
        {"ㄎㄧㄤ", "kiang"},  {"ㄎㄨㄚ", "kua"},    {"ㄎㄨㄛ", "kuo"},
        {"ㄎㄨㄞ", "kuai"},   {"ㄎㄨㄟ", "kui"},    {"ㄎㄨㄢ", "kuan"},
        {"ㄎㄨㄣ", "kun"},    {"ㄎㄨㄤ", "kuang"},  {"ㄎㄨㄥ", "kong"},
        {"ㄏㄨㄚ", "hua"},    {"ㄏㄨㄛ", "huo"},    {"ㄏㄨㄞ", "huai"},
        {"ㄏㄨㄟ", "hui"},    {"ㄏㄨㄢ", "huan"},   {"ㄏㄨㄣ", "hun"},
        {"ㄏㄨㄤ", "huang"},  {"ㄏㄨㄥ", "hong"},   {"ㄐㄧㄚ", "jia"},
        {"ㄐㄧㄝ", "jie"},    {"ㄐㄧㄠ", "jiao"},   {"ㄐㄧㄡ", "jiu"},
        {"ㄐㄧㄢ", "jian"},   {"ㄐㄧㄣ", "jin"},    {"ㄐㄧㄤ", "jiang"},
        {"ㄐㄧㄥ", "jing"},   {"ㄐㄩㄝ", "jue"},    {"ㄐㄩㄢ", "juan"},
        {"ㄐㄩㄣ", "jun"},    {"ㄐㄩㄥ", "jiong"},  {"ㄑㄧㄚ", "qia"},
        {"ㄑㄧㄝ", "qie"},    {"ㄑㄧㄠ", "qiao"},   {"ㄑㄧㄡ", "qiu"},
        {"ㄑㄧㄢ", "qian"},   {"ㄑㄧㄣ", "qin"},    {"ㄑㄧㄤ", "qiang"},
        {"ㄑㄧㄥ", "qing"},   {"ㄑㄩㄝ", "que"},    {"ㄑㄩㄢ", "quan"},
        {"ㄑㄩㄣ", "qun"},    {"ㄑㄩㄥ", "qiong"},  {"ㄒㄧㄚ", "xia"},
        {"ㄒㄧㄝ", "xie"},    {"ㄒㄧㄠ", "xiao"},   {"ㄒㄧㄡ", "xiu"},
        {"ㄒㄧㄢ", "xian"},   {"ㄒㄧㄣ", "xin"},    {"ㄒㄧㄤ", "xiang"},
        {"ㄒㄧㄥ", "xing"},   {"ㄒㄩㄝ", "xue"},    {"ㄒㄩㄢ", "xuan"},
        {"ㄒㄩㄣ", "xun"},    {"ㄒㄩㄥ", "xiong"},  {"ㄓㄨㄚ", "zhua"},
        {"ㄓㄨㄛ", "zhuo"},   {"ㄓㄨㄞ", "zhuai"},  {"ㄓㄨㄟ", "zhui"},
        {"ㄓㄨㄢ", "zhuan"},  {"ㄓㄨㄣ", "zhun"},   {"ㄓㄨㄤ", "zhuang"},
        {"ㄓㄨㄥ", "zhong"},  {"ㄔㄨㄚ", "chua"},   {"ㄔㄨㄛ", "chuo"},
        {"ㄔㄨㄞ", "chuai"},  {"ㄔㄨㄟ", "chui"},   {"ㄔㄨㄢ", "chuan"},
        {"ㄔㄨㄣ", "chun"},   {"ㄔㄨㄤ", "chuang"}, {"ㄔㄨㄥ", "chong"},
        {"ㄕㄨㄚ", "shua"},   {"ㄕㄨㄛ", "shuo"},   {"ㄕㄨㄞ", "shuai"},
        {"ㄕㄨㄟ", "shui"},   {"ㄕㄨㄢ", "shuan"},  {"ㄕㄨㄣ", "shun"},
        {"ㄕㄨㄤ", "shuang"}, {"ㄖㄨㄛ", "ruo"},    {"ㄖㄨㄟ", "rui"},
        {"ㄖㄨㄢ", "ruan"},   {"ㄖㄨㄣ", "run"},    {"ㄖㄨㄥ", "rong"},
        {"ㄗㄨㄛ", "zuo"},    {"ㄗㄨㄟ", "zui"},    {"ㄗㄨㄢ", "zuan"},
        {"ㄗㄨㄣ", "zun"},    {"ㄗㄨㄥ", "zong"},   {"ㄘㄨㄛ", "cuo"},
        {"ㄘㄨㄟ", "cui"},    {"ㄘㄨㄢ", "cuan"},   {"ㄘㄨㄣ", "cun"},
        {"ㄘㄨㄥ", "cong"},   {"ㄙㄨㄛ", "suo"},    {"ㄙㄨㄟ", "sui"},
        {"ㄙㄨㄢ", "suan"},   {"ㄙㄨㄣ", "sun"},    {"ㄙㄨㄥ", "song"},
        {"ㄅㄧㄤ", "biang"},  {"ㄉㄨㄤ", "duang"},

        {"ㄅㄚ", "ba"},       {"ㄅㄛ", "bo"},       {"ㄅㄞ", "bai"},
        {"ㄅㄟ", "bei"},      {"ㄅㄠ", "bao"},      {"ㄅㄢ", "ban"},
        {"ㄅㄣ", "ben"},      {"ㄅㄤ", "bang"},     {"ㄅㄥ", "beng"},
        {"ㄅㄧ", "bi"},       {"ㄅㄨ", "bu"},       {"ㄆㄚ", "pa"},
        {"ㄆㄛ", "po"},       {"ㄆㄞ", "pai"},      {"ㄆㄟ", "pei"},
        {"ㄆㄠ", "pao"},      {"ㄆㄡ", "pou"},      {"ㄆㄢ", "pan"},
        {"ㄆㄣ", "pen"},      {"ㄆㄤ", "pang"},     {"ㄆㄥ", "peng"},
        {"ㄆㄧ", "pi"},       {"ㄆㄨ", "pu"},       {"ㄇㄚ", "ma"},
        {"ㄇㄛ", "mo"},       {"ㄇㄜ", "me"},       {"ㄇㄞ", "mai"},
        {"ㄇㄟ", "mei"},      {"ㄇㄠ", "mao"},      {"ㄇㄡ", "mou"},
        {"ㄇㄢ", "man"},      {"ㄇㄣ", "men"},      {"ㄇㄤ", "mang"},
        {"ㄇㄥ", "meng"},     {"ㄇㄧ", "mi"},       {"ㄇㄨ", "mu"},
        {"ㄈㄚ", "fa"},       {"ㄈㄛ", "fo"},       {"ㄈㄟ", "fei"},
        {"ㄈㄡ", "fou"},      {"ㄈㄢ", "fan"},      {"ㄈㄣ", "fen"},
        {"ㄈㄤ", "fang"},     {"ㄈㄥ", "feng"},     {"ㄈㄨ", "fu"},
        {"ㄉㄚ", "da"},       {"ㄉㄜ", "de"},       {"ㄉㄞ", "dai"},
        {"ㄉㄟ", "dei"},      {"ㄉㄠ", "dao"},      {"ㄉㄡ", "dou"},
        {"ㄉㄢ", "dan"},      {"ㄉㄣ", "den"},      {"ㄉㄤ", "dang"},
        {"ㄉㄥ", "deng"},     {"ㄉㄧ", "di"},       {"ㄉㄨ", "du"},
        {"ㄊㄚ", "ta"},       {"ㄊㄜ", "te"},       {"ㄊㄞ", "tai"},
        {"ㄊㄠ", "tao"},      {"ㄊㄡ", "tou"},      {"ㄊㄢ", "tan"},
        {"ㄊㄤ", "tang"},     {"ㄊㄥ", "teng"},     {"ㄊㄧ", "ti"},
        {"ㄊㄨ", "tu"},       {"ㄋㄚ", "na"},       {"ㄋㄜ", "ne"},
        {"ㄋㄞ", "nai"},      {"ㄋㄟ", "nei"},      {"ㄋㄠ", "nao"},
        {"ㄋㄡ", "nou"},      {"ㄋㄢ", "nan"},      {"ㄋㄣ", "nen"},
        {"ㄋㄤ", "nang"},     {"ㄋㄥ", "neng"},     {"ㄋㄧ", "ni"},
        {"ㄋㄨ", "nu"},       {"ㄋㄩ", "nv"},       {"ㄌㄚ", "la"},
        {"ㄌㄛ", "lo"},       {"ㄌㄜ", "le"},       {"ㄌㄞ", "lai"},
        {"ㄌㄟ", "lei"},      {"ㄌㄠ", "lao"},      {"ㄌㄡ", "lou"},
        {"ㄌㄢ", "lan"},      {"ㄌㄤ", "lang"},     {"ㄌㄥ", "leng"},
        {"ㄌㄧ", "li"},       {"ㄌㄨ", "lu"},       {"ㄌㄩ", "lv"},
        {"ㄍㄚ", "ga"},       {"ㄍㄜ", "ge"},       {"ㄍㄞ", "gai"},
        {"ㄍㄟ", "gei"},      {"ㄍㄠ", "gao"},      {"ㄍㄡ", "gou"},
        {"ㄍㄢ", "gan"},      {"ㄍㄣ", "gen"},      {"ㄍㄤ", "gang"},
        {"ㄍㄥ", "geng"},     {"ㄍㄧ", "gi"},       {"ㄍㄨ", "gu"},
        {"ㄎㄚ", "ka"},       {"ㄎㄜ", "ke"},       {"ㄎㄞ", "kai"},
        {"ㄎㄠ", "kao"},      {"ㄎㄡ", "kou"},      {"ㄎㄢ", "kan"},
        {"ㄎㄣ", "ken"},      {"ㄎㄤ", "kang"},     {"ㄎㄥ", "keng"},
        {"ㄎㄨ", "ku"},       {"ㄏㄚ", "ha"},       {"ㄏㄜ", "he"},
        {"ㄏㄞ", "hai"},      {"ㄏㄟ", "hei"},      {"ㄏㄠ", "hao"},
        {"ㄏㄡ", "hou"},      {"ㄏㄢ", "han"},      {"ㄏㄣ", "hen"},
        {"ㄏㄤ", "hang"},     {"ㄏㄥ", "heng"},     {"ㄏㄨ", "hu"},
        {"ㄐㄧ", "ji"},       {"ㄐㄩ", "ju"},       {"ㄑㄧ", "qi"},
        {"ㄑㄩ", "qu"},       {"ㄒㄧ", "xi"},       {"ㄒㄩ", "xu"},
        {"ㄓㄚ", "zha"},      {"ㄓㄜ", "zhe"},      {"ㄓㄞ", "zhai"},
        {"ㄓㄟ", "zhei"},     {"ㄓㄠ", "zhao"},     {"ㄓㄡ", "zhou"},
        {"ㄓㄢ", "zhan"},     {"ㄓㄣ", "zhen"},     {"ㄓㄤ", "zhang"},
        {"ㄓㄥ", "zheng"},    {"ㄓㄨ", "zhu"},      {"ㄔㄚ", "cha"},
        {"ㄔㄜ", "che"},      {"ㄔㄞ", "chai"},     {"ㄔㄠ", "chao"},
        {"ㄔㄡ", "chou"},     {"ㄔㄢ", "chan"},     {"ㄔㄣ", "chen"},
        {"ㄔㄤ", "chang"},    {"ㄔㄥ", "cheng"},    {"ㄔㄨ", "chu"},
        {"ㄕㄚ", "sha"},      {"ㄕㄜ", "she"},      {"ㄕㄞ", "shai"},
        {"ㄕㄟ", "shei"},     {"ㄕㄠ", "shao"},     {"ㄕㄡ", "shou"},
        {"ㄕㄢ", "shan"},     {"ㄕㄣ", "shen"},     {"ㄕㄤ", "shang"},
        {"ㄕㄥ", "sheng"},    {"ㄕㄨ", "shu"},      {"ㄖㄜ", "re"},
        {"ㄖㄠ", "rao"},      {"ㄖㄡ", "rou"},      {"ㄖㄢ", "ran"},
        {"ㄖㄣ", "ren"},      {"ㄖㄤ", "rang"},     {"ㄖㄥ", "reng"},
        {"ㄖㄨ", "ru"},       {"ㄗㄚ", "za"},       {"ㄗㄜ", "ze"},
        {"ㄗㄞ", "zai"},      {"ㄗㄟ", "zei"},      {"ㄗㄠ", "zao"},
        {"ㄗㄡ", "zou"},      {"ㄗㄢ", "zan"},      {"ㄗㄣ", "zen"},
        {"ㄗㄤ", "zang"},     {"ㄗㄥ", "zeng"},     {"ㄗㄨ", "zu"},
        {"ㄘㄚ", "ca"},       {"ㄘㄜ", "ce"},       {"ㄘㄞ", "cai"},
        {"ㄘㄟ", "cei"},      {"ㄘㄠ", "cao"},      {"ㄘㄡ", "cou"},
        {"ㄘㄢ", "can"},      {"ㄘㄣ", "cen"},      {"ㄘㄤ", "cang"},
        {"ㄘㄥ", "ceng"},     {"ㄘㄨ", "cu"},       {"ㄙㄚ", "sa"},
        {"ㄙㄜ", "se"},       {"ㄙㄞ", "sai"},      {"ㄙㄟ", "sei"},
        {"ㄙㄠ", "sao"},      {"ㄙㄡ", "sou"},      {"ㄙㄢ", "san"},
        {"ㄙㄣ", "sen"},      {"ㄙㄤ", "sang"},     {"ㄙㄥ", "seng"},
        {"ㄙㄨ", "su"},       {"ㄧㄚ", "ya"},       {"ㄧㄛ", "yo"},
        {"ㄧㄝ", "ye"},       {"ㄧㄞ", "yai"},      {"ㄧㄠ", "yao"},
        {"ㄧㄡ", "you"},      {"ㄧㄢ", "yan"},      {"ㄧㄣ", "yin"},
        {"ㄧㄤ", "yang"},     {"ㄧㄥ", "ying"},     {"ㄨㄚ", "wa"},
        {"ㄨㄛ", "wo"},       {"ㄨㄞ", "wai"},      {"ㄨㄟ", "wei"},
        {"ㄨㄢ", "wan"},      {"ㄨㄣ", "wen"},      {"ㄨㄤ", "wang"},
        {"ㄨㄥ", "weng"},     {"ㄩㄝ", "yue"},      {"ㄩㄢ", "yuan"},
        {"ㄩㄣ", "yun"},      {"ㄩㄥ", "yong"},

        {"ㄅ", "b"},          {"ㄆ", "p"},          {"ㄇ", "m"},
        {"ㄈ", "f"},          {"ㄉ", "d"},          {"ㄊ", "t"},
        {"ㄋ", "n"},          {"ㄌ", "l"},          {"ㄍ", "g"},
        {"ㄎ", "k"},          {"ㄏ", "h"},          {"ㄐ", "j"},
        {"ㄑ", "q"},          {"ㄒ", "x"},          {"ㄓ", "zhi"},
        {"ㄔ", "chi"},        {"ㄕ", "shi"},        {"ㄖ", "ri"},
        {"ㄗ", "zi"},         {"ㄘ", "ci"},         {"ㄙ", "si"},
        {"ㄚ", "a"},          {"ㄛ", "o"},          {"ㄜ", "e"},
        {"ㄝ", "eh"},         {"ㄞ", "ai"},         {"ㄟ", "ei"},
        {"ㄠ", "ao"},         {"ㄡ", "ou"},         {"ㄢ", "an"},
        {"ㄣ", "en"},         {"ㄤ", "ang"},        {"ㄥ", "eng"},
        {"ㄦ", "er"},         {"ㄧ", "yi"},         {"ㄨ", "wu"},
        {"ㄩ", "yu"}};

/// 漢語拼音韻母轉換對照表資料貯存專用佇列
inline static std::vector<std::vector<std::string>>
    arrHanyuPinyinTextbookStyleConversionTable =
        {  // 排序很重要。先處理最長的，再處理短的。不然會出亂子。
            {"iang1", "iāng"}, {"iang2", "iáng"}, {"iang3", "iǎng"},
            {"iang4", "iàng"}, {"iong1", "iōng"}, {"iong2", "ióng"},
            {"iong3", "iǒng"}, {"iong4", "iòng"}, {"uang1", "uāng"},
            {"uang2", "uáng"}, {"uang3", "uǎng"}, {"uang4", "uàng"},
            {"uang5", "uang"},

            {"ang1", "āng"},   {"ang2", "áng"},   {"ang3", "ǎng"},
            {"ang4", "àng"},   {"ang5", "ang"},   {"eng1", "ēng"},
            {"eng2", "éng"},   {"eng3", "ěng"},   {"eng4", "èng"},
            {"ian1", "iān"},   {"ian2", "ián"},   {"ian3", "iǎn"},
            {"ian4", "iàn"},   {"iao1", "iāo"},   {"iao2", "iáo"},
            {"iao3", "iǎo"},   {"iao4", "iào"},   {"ing1", "īng"},
            {"ing2", "íng"},   {"ing3", "ǐng"},   {"ing4", "ìng"},
            {"ong1", "ōng"},   {"ong2", "óng"},   {"ong3", "ǒng"},
            {"ong4", "òng"},   {"uai1", "uāi"},   {"uai2", "uái"},
            {"uai3", "uǎi"},   {"uai4", "uài"},   {"uan1", "uān"},
            {"uan2", "uán"},   {"uan3", "uǎn"},   {"uan4", "uàn"},
            {"van2", "üán"},   {"van3", "üǎn"},

            {"ai1", "āi"},     {"ai2", "ái"},     {"ai3", "ǎi"},
            {"ai4", "ài"},     {"ai5", "ai"},     {"an1", "ān"},
            {"an2", "án"},     {"an3", "ǎn"},     {"an4", "àn"},
            {"ao1", "āo"},     {"ao2", "áo"},     {"ao3", "ǎo"},
            {"ao4", "ào"},     {"ao5", "ao"},     {"eh2", "ế"},
            {"eh3", "êˇ"},     {"eh4", "ề"},      {"eh5", "ê"},
            {"ei1", "ēi"},     {"ei2", "éi"},     {"ei3", "ěi"},
            {"ei4", "èi"},     {"ei5", "ei"},     {"en1", "ēn"},
            {"en2", "én"},     {"en3", "ěn"},     {"en4", "èn"},
            {"en5", "en"},     {"er1", "ēr"},     {"er2", "ér"},
            {"er3", "ěr"},     {"er4", "èr"},     {"er5", "er"},
            {"ia1", "iā"},     {"ia2", "iá"},     {"ia3", "iǎ"},
            {"ia4", "ià"},     {"ie1", "iē"},     {"ie2", "ié"},
            {"ie3", "iě"},     {"ie4", "iè"},     {"ie5", "ie"},
            {"in1", "īn"},     {"in2", "ín"},     {"in3", "ǐn"},
            {"in4", "ìn"},     {"iu1", "iū"},     {"iu2", "iú"},
            {"iu3", "iǔ"},     {"iu4", "iù"},     {"ou1", "ōu"},
            {"ou2", "óu"},     {"ou3", "ǒu"},     {"ou4", "òu"},
            {"ou5", "ou"},     {"ua1", "uā"},     {"ua2", "uá"},
            {"ua3", "uǎ"},     {"ua4", "uà"},     {"ue1", "uē"},
            {"ue2", "ué"},     {"ue3", "uě"},     {"ue4", "uè"},
            {"ui1", "uī"},     {"ui2", "uí"},     {"ui3", "uǐ"},
            {"ui4", "uì"},     {"un1", "ūn"},     {"un2", "ún"},
            {"un3", "ǔn"},     {"un4", "ùn"},     {"uo1", "uō"},
            {"uo2", "uó"},     {"uo3", "uǒ"},     {"uo4", "uò"},
            {"uo5", "uo"},     {"ve1", "üē"},     {"ve3", "üě"},
            {"ve4", "üè"},

            {"a1", "ā"},       {"a2", "á"},       {"a3", "ǎ"},
            {"a4", "à"},       {"a5", "a"},       {"e1", "ē"},
            {"e2", "é"},       {"e3", "ě"},       {"e4", "è"},
            {"e5", "e"},       {"i1", "ī"},       {"i2", "í"},
            {"i3", "ǐ"},       {"i4", "ì"},       {"i5", "i"},
            {"o1", "ō"},       {"o2", "ó"},       {"o3", "ǒ"},
            {"o4", "ò"},       {"o5", "o"},       {"u1", "ū"},
            {"u2", "ú"},       {"u3", "ǔ"},       {"u4", "ù"},
            {"v1", "ǖ"},       {"v2", "ǘ"},       {"v3", "ǚ"},
            {"v4", "ǜ"}};

// MARK: - Maps for Keyboard-to-Pinyin parsers

/// 任何形式的拼音排列都會用到的陣列（韋氏拼音與趙元任國語羅馬字除外），
/// 用 Strings 反而省事一些。
/// 這裡同時兼容大千注音的調號數字，所以也將 6、7 號數字鍵放在允許範圍內。
inline static std::string mapArayuruPinyin =
    "abcdefghijklmnopqrstuvwxyz1234567 ";

/// 任何形式的拼音排列都會用到的陣列（韋氏拼音與趙元任國語羅馬字除外），
/// 用 Strings 反而省事一些。
/// 這裡同時兼容大千注音的調號數字，所以也將 6、7 號數字鍵放在允許範圍內。
inline static std::string mapWadeGilesPinyinKeys = mapArayuruPinyin + "'";

/// 任何拼音都會用到的聲調鍵陣列
inline static std::map<std::string, std::string> mapArayuruPinyinIntonation = {
    {"1", " "}, {"2", "ˊ"}, {"3", "ˇ"}, {"4", "ˋ"},
    {"5", "˙"}, {"6", "ˊ"}, {"7", "˙"}, {" ", " "}};

/// 漢語拼音排列專用處理陣列
inline static std::map<std::string, std::string> mapHanyuPinyin = {
    {"chuang", "ㄔㄨㄤ"}, {"shuang", "ㄕㄨㄤ"}, {"zhuang", "ㄓㄨㄤ"},
    {"chang", "ㄔㄤ"},    {"cheng", "ㄔㄥ"},    {"chong", "ㄔㄨㄥ"},
    {"chuai", "ㄔㄨㄞ"},  {"chuan", "ㄔㄨㄢ"},  {"guang", "ㄍㄨㄤ"},
    {"huang", "ㄏㄨㄤ"},  {"jiang", "ㄐㄧㄤ"},  {"jiong", "ㄐㄩㄥ"},
    {"kiang", "ㄎㄧㄤ"},  {"kuang", "ㄎㄨㄤ"},  {"biang", "ㄅㄧㄤ"},
    {"duang", "ㄉㄨㄤ"},  {"liang", "ㄌㄧㄤ"},  {"niang", "ㄋㄧㄤ"},
    {"qiang", "ㄑㄧㄤ"},  {"qiong", "ㄑㄩㄥ"},  {"shang", "ㄕㄤ"},
    {"sheng", "ㄕㄥ"},    {"shuai", "ㄕㄨㄞ"},  {"shuan", "ㄕㄨㄢ"},
    {"xiang", "ㄒㄧㄤ"},  {"xiong", "ㄒㄩㄥ"},  {"zhang", "ㄓㄤ"},
    {"zheng", "ㄓㄥ"},    {"zhong", "ㄓㄨㄥ"},  {"zhuai", "ㄓㄨㄞ"},
    {"zhuan", "ㄓㄨㄢ"},  {"bang", "ㄅㄤ"},     {"beng", "ㄅㄥ"},
    {"bian", "ㄅㄧㄢ"},   {"biao", "ㄅㄧㄠ"},   {"bing", "ㄅㄧㄥ"},
    {"cang", "ㄘㄤ"},     {"ceng", "ㄘㄥ"},     {"chai", "ㄔㄞ"},
    {"chan", "ㄔㄢ"},     {"chao", "ㄔㄠ"},     {"chen", "ㄔㄣ"},
    {"chou", "ㄔㄡ"},     {"chua", "ㄔㄨㄚ"},   {"chui", "ㄔㄨㄟ"},
    {"chun", "ㄔㄨㄣ"},   {"chuo", "ㄔㄨㄛ"},   {"cong", "ㄘㄨㄥ"},
    {"cuan", "ㄘㄨㄢ"},   {"dang", "ㄉㄤ"},     {"deng", "ㄉㄥ"},
    {"dian", "ㄉㄧㄢ"},   {"diao", "ㄉㄧㄠ"},   {"ding", "ㄉㄧㄥ"},
    {"dong", "ㄉㄨㄥ"},   {"duan", "ㄉㄨㄢ"},   {"fang", "ㄈㄤ"},
    {"feng", "ㄈㄥ"},     {"fiao", "ㄈㄧㄠ"},   {"fong", "ㄈㄨㄥ"},
    {"gang", "ㄍㄤ"},     {"geng", "ㄍㄥ"},     {"giao", "ㄍㄧㄠ"},
    {"gong", "ㄍㄨㄥ"},   {"guai", "ㄍㄨㄞ"},   {"guan", "ㄍㄨㄢ"},
    {"hang", "ㄏㄤ"},     {"heng", "ㄏㄥ"},     {"hong", "ㄏㄨㄥ"},
    {"huai", "ㄏㄨㄞ"},   {"huan", "ㄏㄨㄢ"},   {"jian", "ㄐㄧㄢ"},
    {"jiao", "ㄐㄧㄠ"},   {"jing", "ㄐㄧㄥ"},   {"juan", "ㄐㄩㄢ"},
    {"kang", "ㄎㄤ"},     {"keng", "ㄎㄥ"},     {"kong", "ㄎㄨㄥ"},
    {"kuai", "ㄎㄨㄞ"},   {"kuan", "ㄎㄨㄢ"},   {"lang", "ㄌㄤ"},
    {"leng", "ㄌㄥ"},     {"lian", "ㄌㄧㄢ"},   {"liao", "ㄌㄧㄠ"},
    {"ling", "ㄌㄧㄥ"},   {"long", "ㄌㄨㄥ"},   {"luan", "ㄌㄨㄢ"},
    {"lvan", "ㄌㄩㄢ"},   {"mang", "ㄇㄤ"},     {"meng", "ㄇㄥ"},
    {"mian", "ㄇㄧㄢ"},   {"miao", "ㄇㄧㄠ"},   {"ming", "ㄇㄧㄥ"},
    {"nang", "ㄋㄤ"},     {"neng", "ㄋㄥ"},     {"nian", "ㄋㄧㄢ"},
    {"niao", "ㄋㄧㄠ"},   {"ning", "ㄋㄧㄥ"},   {"nong", "ㄋㄨㄥ"},
    {"nuan", "ㄋㄨㄢ"},   {"pang", "ㄆㄤ"},     {"peng", "ㄆㄥ"},
    {"pian", "ㄆㄧㄢ"},   {"piao", "ㄆㄧㄠ"},   {"ping", "ㄆㄧㄥ"},
    {"qian", "ㄑㄧㄢ"},   {"qiao", "ㄑㄧㄠ"},   {"qing", "ㄑㄧㄥ"},
    {"quan", "ㄑㄩㄢ"},   {"rang", "ㄖㄤ"},     {"reng", "ㄖㄥ"},
    {"rong", "ㄖㄨㄥ"},   {"ruan", "ㄖㄨㄢ"},   {"sang", "ㄙㄤ"},
    {"seng", "ㄙㄥ"},     {"shai", "ㄕㄞ"},     {"shan", "ㄕㄢ"},
    {"shao", "ㄕㄠ"},     {"shei", "ㄕㄟ"},     {"shen", "ㄕㄣ"},
    {"shou", "ㄕㄡ"},     {"shua", "ㄕㄨㄚ"},   {"shui", "ㄕㄨㄟ"},
    {"shun", "ㄕㄨㄣ"},   {"shuo", "ㄕㄨㄛ"},   {"song", "ㄙㄨㄥ"},
    {"suan", "ㄙㄨㄢ"},   {"tang", "ㄊㄤ"},     {"teng", "ㄊㄥ"},
    {"tian", "ㄊㄧㄢ"},   {"tiao", "ㄊㄧㄠ"},   {"ting", "ㄊㄧㄥ"},
    {"tong", "ㄊㄨㄥ"},   {"tuan", "ㄊㄨㄢ"},   {"wang", "ㄨㄤ"},
    {"weng", "ㄨㄥ"},     {"xian", "ㄒㄧㄢ"},   {"xiao", "ㄒㄧㄠ"},
    {"xing", "ㄒㄧㄥ"},   {"xuan", "ㄒㄩㄢ"},   {"yang", "ㄧㄤ"},
    {"ying", "ㄧㄥ"},     {"yong", "ㄩㄥ"},     {"yuan", "ㄩㄢ"},
    {"zang", "ㄗㄤ"},     {"zeng", "ㄗㄥ"},     {"zhai", "ㄓㄞ"},
    {"zhan", "ㄓㄢ"},     {"zhao", "ㄓㄠ"},     {"zhei", "ㄓㄟ"},
    {"zhen", "ㄓㄣ"},     {"zhou", "ㄓㄡ"},     {"zhua", "ㄓㄨㄚ"},
    {"zhui", "ㄓㄨㄟ"},   {"zhun", "ㄓㄨㄣ"},   {"zhuo", "ㄓㄨㄛ"},
    {"zong", "ㄗㄨㄥ"},   {"zuan", "ㄗㄨㄢ"},   {"jun", "ㄐㄩㄣ"},
    {"ang", "ㄤ"},        {"bai", "ㄅㄞ"},      {"ban", "ㄅㄢ"},
    {"bao", "ㄅㄠ"},      {"bei", "ㄅㄟ"},      {"ben", "ㄅㄣ"},
    {"bie", "ㄅㄧㄝ"},    {"bin", "ㄅㄧㄣ"},    {"cai", "ㄘㄞ"},
    {"can", "ㄘㄢ"},      {"cao", "ㄘㄠ"},      {"cei", "ㄘㄟ"},
    {"cen", "ㄘㄣ"},      {"cha", "ㄔㄚ"},      {"che", "ㄔㄜ"},
    {"chi", "ㄔ"},        {"chu", "ㄔㄨ"},      {"cou", "ㄘㄡ"},
    {"cui", "ㄘㄨㄟ"},    {"cun", "ㄘㄨㄣ"},    {"cuo", "ㄘㄨㄛ"},
    {"dai", "ㄉㄞ"},      {"dan", "ㄉㄢ"},      {"dao", "ㄉㄠ"},
    {"dei", "ㄉㄟ"},      {"den", "ㄉㄣ"},      {"dia", "ㄉㄧㄚ"},
    {"die", "ㄉㄧㄝ"},    {"diu", "ㄉㄧㄡ"},    {"dou", "ㄉㄡ"},
    {"dui", "ㄉㄨㄟ"},    {"dun", "ㄉㄨㄣ"},    {"duo", "ㄉㄨㄛ"},
    {"eng", "ㄥ"},        {"fan", "ㄈㄢ"},      {"fei", "ㄈㄟ"},
    {"fen", "ㄈㄣ"},      {"fou", "ㄈㄡ"},      {"gai", "ㄍㄞ"},
    {"gan", "ㄍㄢ"},      {"gao", "ㄍㄠ"},      {"gei", "ㄍㄟ"},
    {"gin", "ㄍㄧㄣ"},    {"gen", "ㄍㄣ"},      {"gou", "ㄍㄡ"},
    {"gua", "ㄍㄨㄚ"},    {"gue", "ㄍㄨㄜ"},    {"gui", "ㄍㄨㄟ"},
    {"gun", "ㄍㄨㄣ"},    {"guo", "ㄍㄨㄛ"},    {"hai", "ㄏㄞ"},
    {"han", "ㄏㄢ"},      {"hao", "ㄏㄠ"},      {"hei", "ㄏㄟ"},
    {"hen", "ㄏㄣ"},      {"hou", "ㄏㄡ"},      {"hua", "ㄏㄨㄚ"},
    {"hui", "ㄏㄨㄟ"},    {"hun", "ㄏㄨㄣ"},    {"huo", "ㄏㄨㄛ"},
    {"jia", "ㄐㄧㄚ"},    {"jie", "ㄐㄧㄝ"},    {"jin", "ㄐㄧㄣ"},
    {"jiu", "ㄐㄧㄡ"},    {"jue", "ㄐㄩㄝ"},    {"kai", "ㄎㄞ"},
    {"kan", "ㄎㄢ"},      {"kao", "ㄎㄠ"},      {"ken", "ㄎㄣ"},
    {"kiu", "ㄎㄧㄡ"},    {"kou", "ㄎㄡ"},      {"kua", "ㄎㄨㄚ"},
    {"kui", "ㄎㄨㄟ"},    {"kun", "ㄎㄨㄣ"},    {"kuo", "ㄎㄨㄛ"},
    {"lai", "ㄌㄞ"},      {"lan", "ㄌㄢ"},      {"lao", "ㄌㄠ"},
    {"lei", "ㄌㄟ"},      {"lia", "ㄌㄧㄚ"},    {"lie", "ㄌㄧㄝ"},
    {"lin", "ㄌㄧㄣ"},    {"liu", "ㄌㄧㄡ"},    {"lou", "ㄌㄡ"},
    {"lun", "ㄌㄨㄣ"},    {"luo", "ㄌㄨㄛ"},    {"lve", "ㄌㄩㄝ"},
    {"mai", "ㄇㄞ"},      {"man", "ㄇㄢ"},      {"mao", "ㄇㄠ"},
    {"mei", "ㄇㄟ"},      {"men", "ㄇㄣ"},      {"mie", "ㄇㄧㄝ"},
    {"min", "ㄇㄧㄣ"},    {"miu", "ㄇㄧㄡ"},    {"mou", "ㄇㄡ"},
    {"nai", "ㄋㄞ"},      {"nan", "ㄋㄢ"},      {"nao", "ㄋㄠ"},
    {"nei", "ㄋㄟ"},      {"nen", "ㄋㄣ"},      {"nie", "ㄋㄧㄝ"},
    {"nin", "ㄋㄧㄣ"},    {"niu", "ㄋㄧㄡ"},    {"nou", "ㄋㄡ"},
    {"nui", "ㄋㄨㄟ"},    {"nun", "ㄋㄨㄣ"},    {"nuo", "ㄋㄨㄛ"},
    {"nve", "ㄋㄩㄝ"},    {"pai", "ㄆㄞ"},      {"pan", "ㄆㄢ"},
    {"pao", "ㄆㄠ"},      {"pei", "ㄆㄟ"},      {"pen", "ㄆㄣ"},
    {"pia", "ㄆㄧㄚ"},    {"pie", "ㄆㄧㄝ"},    {"pin", "ㄆㄧㄣ"},
    {"pou", "ㄆㄡ"},      {"qia", "ㄑㄧㄚ"},    {"qie", "ㄑㄧㄝ"},
    {"qin", "ㄑㄧㄣ"},    {"qiu", "ㄑㄧㄡ"},    {"que", "ㄑㄩㄝ"},
    {"qun", "ㄑㄩㄣ"},    {"ran", "ㄖㄢ"},      {"rao", "ㄖㄠ"},
    {"ren", "ㄖㄣ"},      {"rou", "ㄖㄡ"},      {"rui", "ㄖㄨㄟ"},
    {"run", "ㄖㄨㄣ"},    {"ruo", "ㄖㄨㄛ"},    {"sai", "ㄙㄞ"},
    {"san", "ㄙㄢ"},      {"sao", "ㄙㄠ"},      {"sei", "ㄙㄟ"},
    {"sen", "ㄙㄣ"},      {"sha", "ㄕㄚ"},      {"she", "ㄕㄜ"},
    {"shi", "ㄕ"},        {"shu", "ㄕㄨ"},      {"sou", "ㄙㄡ"},
    {"sui", "ㄙㄨㄟ"},    {"sun", "ㄙㄨㄣ"},    {"suo", "ㄙㄨㄛ"},
    {"tai", "ㄊㄞ"},      {"tan", "ㄊㄢ"},      {"tao", "ㄊㄠ"},
    {"tie", "ㄊㄧㄝ"},    {"tou", "ㄊㄡ"},      {"tui", "ㄊㄨㄟ"},
    {"tun", "ㄊㄨㄣ"},    {"tuo", "ㄊㄨㄛ"},    {"wai", "ㄨㄞ"},
    {"wan", "ㄨㄢ"},      {"wei", "ㄨㄟ"},      {"wen", "ㄨㄣ"},
    {"xia", "ㄒㄧㄚ"},    {"xie", "ㄒㄧㄝ"},    {"xin", "ㄒㄧㄣ"},
    {"xiu", "ㄒㄧㄡ"},    {"xue", "ㄒㄩㄝ"},    {"xun", "ㄒㄩㄣ"},
    {"yai", "ㄧㄞ"},      {"yan", "ㄧㄢ"},      {"yao", "ㄧㄠ"},
    {"yin", "ㄧㄣ"},      {"you", "ㄧㄡ"},      {"yue", "ㄩㄝ"},
    {"yun", "ㄩㄣ"},      {"zai", "ㄗㄞ"},      {"zan", "ㄗㄢ"},
    {"zao", "ㄗㄠ"},      {"zei", "ㄗㄟ"},      {"zen", "ㄗㄣ"},
    {"zha", "ㄓㄚ"},      {"zhe", "ㄓㄜ"},      {"zhi", "ㄓ"},
    {"zhu", "ㄓㄨ"},      {"zou", "ㄗㄡ"},      {"zui", "ㄗㄨㄟ"},
    {"zun", "ㄗㄨㄣ"},    {"zuo", "ㄗㄨㄛ"},    {"ai", "ㄞ"},
    {"an", "ㄢ"},         {"ao", "ㄠ"},         {"ba", "ㄅㄚ"},
    {"bi", "ㄅㄧ"},       {"bo", "ㄅㄛ"},       {"bu", "ㄅㄨ"},
    {"ca", "ㄘㄚ"},       {"ce", "ㄘㄜ"},       {"ci", "ㄘ"},
    {"cu", "ㄘㄨ"},       {"da", "ㄉㄚ"},       {"de", "ㄉㄜ"},
    {"di", "ㄉㄧ"},       {"du", "ㄉㄨ"},       {"eh", "ㄝ"},
    {"ei", "ㄟ"},         {"en", "ㄣ"},         {"er", "ㄦ"},
    {"fa", "ㄈㄚ"},       {"fo", "ㄈㄛ"},       {"fu", "ㄈㄨ"},
    {"ga", "ㄍㄚ"},       {"ge", "ㄍㄜ"},       {"gi", "ㄍㄧ"},
    {"gu", "ㄍㄨ"},       {"ha", "ㄏㄚ"},       {"he", "ㄏㄜ"},
    {"hu", "ㄏㄨ"},       {"ji", "ㄐㄧ"},       {"ju", "ㄐㄩ"},
    {"ka", "ㄎㄚ"},       {"ke", "ㄎㄜ"},       {"ku", "ㄎㄨ"},
    {"la", "ㄌㄚ"},       {"le", "ㄌㄜ"},       {"li", "ㄌㄧ"},
    {"lo", "ㄌㄛ"},       {"lu", "ㄌㄨ"},       {"lv", "ㄌㄩ"},
    {"ma", "ㄇㄚ"},       {"me", "ㄇㄜ"},       {"mi", "ㄇㄧ"},
    {"mo", "ㄇㄛ"},       {"mu", "ㄇㄨ"},       {"na", "ㄋㄚ"},
    {"ne", "ㄋㄜ"},       {"ni", "ㄋㄧ"},       {"nu", "ㄋㄨ"},
    {"nv", "ㄋㄩ"},       {"ou", "ㄡ"},         {"pa", "ㄆㄚ"},
    {"pi", "ㄆㄧ"},       {"po", "ㄆㄛ"},       {"pu", "ㄆㄨ"},
    {"qi", "ㄑㄧ"},       {"qu", "ㄑㄩ"},       {"re", "ㄖㄜ"},
    {"ri", "ㄖ"},         {"ru", "ㄖㄨ"},       {"sa", "ㄙㄚ"},
    {"se", "ㄙㄜ"},       {"si", "ㄙ"},         {"su", "ㄙㄨ"},
    {"ta", "ㄊㄚ"},       {"te", "ㄊㄜ"},       {"ti", "ㄊㄧ"},
    {"tu", "ㄊㄨ"},       {"wa", "ㄨㄚ"},       {"wo", "ㄨㄛ"},
    {"wu", "ㄨ"},         {"xi", "ㄒㄧ"},       {"xu", "ㄒㄩ"},
    {"ya", "ㄧㄚ"},       {"ye", "ㄧㄝ"},       {"yi", "ㄧ"},
    {"yo", "ㄧㄛ"},       {"yu", "ㄩ"},         {"za", "ㄗㄚ"},
    {"ze", "ㄗㄜ"},       {"zi", "ㄗ"},         {"zu", "ㄗㄨ"},
    {"a", "ㄚ"},          {"e", "ㄜ"},          {"o", "ㄛ"},
    {"q", "ㄑ"}};

/// 國音二式排列專用處理陣列
inline static std::map<std::string, std::string> mapSecondaryPinyin = {
    {"chuang", "ㄔㄨㄤ"}, {"shuang", "ㄕㄨㄤ"}, {"chiang", "ㄑㄧㄤ"},
    {"chiung", "ㄑㄩㄥ"}, {"chiuan", "ㄑㄩㄢ"}, {"shiang", "ㄒㄧㄤ"},
    {"shiung", "ㄒㄩㄥ"}, {"shiuan", "ㄒㄩㄢ"}, {"biang", "ㄅㄧㄤ"},
    {"duang", "ㄉㄨㄤ"},  {"juang", "ㄓㄨㄤ"},  {"jiang", "ㄐㄧㄤ"},
    {"jiung", "ㄐㄩㄥ"},  {"niang", "ㄋㄧㄤ"},  {"liang", "ㄌㄧㄤ"},
    {"guang", "ㄍㄨㄤ"},  {"kuang", "ㄎㄨㄤ"},  {"huang", "ㄏㄨㄤ"},
    {"chang", "ㄔㄤ"},    {"cheng", "ㄔㄥ"},    {"chuai", "ㄔㄨㄞ"},
    {"chuan", "ㄔㄨㄢ"},  {"chung", "ㄔㄨㄥ"},  {"shang", "ㄕㄤ"},
    {"sheng", "ㄕㄥ"},    {"shuai", "ㄕㄨㄞ"},  {"shuan", "ㄕㄨㄢ"},
    {"jiuan", "ㄐㄩㄢ"},  {"chiau", "ㄑㄧㄠ"},  {"chian", "ㄑㄧㄢ"},
    {"ching", "ㄑㄧㄥ"},  {"shing", "ㄒㄧㄥ"},  {"tzang", "ㄗㄤ"},
    {"tzeng", "ㄗㄥ"},    {"tzuan", "ㄗㄨㄢ"},  {"tzung", "ㄗㄨㄥ"},
    {"tsang", "ㄘㄤ"},    {"tseng", "ㄘㄥ"},    {"tsuan", "ㄘㄨㄢ"},
    {"tsung", "ㄘㄨㄥ"},  {"chiue", "ㄑㄩㄝ"},  {"liuan", "ㄌㄩㄢ"},
    {"chuei", "ㄔㄨㄟ"},  {"chuen", "ㄔㄨㄣ"},  {"shuei", "ㄕㄨㄟ"},
    {"shuen", "ㄕㄨㄣ"},  {"chiou", "ㄑㄧㄡ"},  {"chiun", "ㄑㄩㄣ"},
    {"tzuei", "ㄗㄨㄟ"},  {"tzuen", "ㄗㄨㄣ"},  {"tsuei", "ㄘㄨㄟ"},
    {"tsuen", "ㄘㄨㄣ"},  {"kiang", "ㄎㄧㄤ"},  {"shiau", "ㄒㄧㄠ"},
    {"shian", "ㄒㄧㄢ"},  {"shiue", "ㄒㄩㄝ"},  {"shiou", "ㄒㄧㄡ"},
    {"shiun", "ㄒㄩㄣ"},  {"jang", "ㄓㄤ"},     {"jeng", "ㄓㄥ"},
    {"juai", "ㄓㄨㄞ"},   {"juan", "ㄓㄨㄢ"},   {"jung", "ㄓㄨㄥ"},
    {"jiau", "ㄐㄧㄠ"},   {"jian", "ㄐㄧㄢ"},   {"jing", "ㄐㄧㄥ"},
    {"jiue", "ㄐㄩㄝ"},   {"chie", "ㄑㄧㄝ"},   {"bang", "ㄅㄤ"},
    {"beng", "ㄅㄥ"},     {"biau", "ㄅㄧㄠ"},   {"bian", "ㄅㄧㄢ"},
    {"bing", "ㄅㄧㄥ"},   {"pang", "ㄆㄤ"},     {"peng", "ㄆㄥ"},
    {"piau", "ㄆㄧㄠ"},   {"pian", "ㄆㄧㄢ"},   {"ping", "ㄆㄧㄥ"},
    {"mang", "ㄇㄤ"},     {"meng", "ㄇㄥ"},     {"miau", "ㄇㄧㄠ"},
    {"mian", "ㄇㄧㄢ"},   {"ming", "ㄇㄧㄥ"},   {"fang", "ㄈㄤ"},
    {"feng", "ㄈㄥ"},     {"fiau", "ㄈㄧㄠ"},   {"dang", "ㄉㄤ"},
    {"deng", "ㄉㄥ"},     {"diau", "ㄉㄧㄠ"},   {"dian", "ㄉㄧㄢ"},
    {"ding", "ㄉㄧㄥ"},   {"duan", "ㄉㄨㄢ"},   {"dung", "ㄉㄨㄥ"},
    {"tang", "ㄊㄤ"},     {"teng", "ㄊㄥ"},     {"tiau", "ㄊㄧㄠ"},
    {"tian", "ㄊㄧㄢ"},   {"ting", "ㄊㄧㄥ"},   {"tuan", "ㄊㄨㄢ"},
    {"tung", "ㄊㄨㄥ"},   {"nang", "ㄋㄤ"},     {"neng", "ㄋㄥ"},
    {"niau", "ㄋㄧㄠ"},   {"nian", "ㄋㄧㄢ"},   {"ning", "ㄋㄧㄥ"},
    {"nuan", "ㄋㄨㄢ"},   {"nung", "ㄋㄨㄥ"},   {"lang", "ㄌㄤ"},
    {"leng", "ㄌㄥ"},     {"liau", "ㄌㄧㄠ"},   {"lian", "ㄌㄧㄢ"},
    {"ling", "ㄌㄧㄥ"},   {"luan", "ㄌㄨㄢ"},   {"lung", "ㄌㄨㄥ"},
    {"gang", "ㄍㄤ"},     {"geng", "ㄍㄥ"},     {"guai", "ㄍㄨㄞ"},
    {"guan", "ㄍㄨㄢ"},   {"gung", "ㄍㄨㄥ"},   {"kang", "ㄎㄤ"},
    {"keng", "ㄎㄥ"},     {"kuai", "ㄎㄨㄞ"},   {"kuan", "ㄎㄨㄢ"},
    {"kung", "ㄎㄨㄥ"},   {"hang", "ㄏㄤ"},     {"heng", "ㄏㄥ"},
    {"huai", "ㄏㄨㄞ"},   {"huan", "ㄏㄨㄢ"},   {"hung", "ㄏㄨㄥ"},
    {"juei", "ㄓㄨㄟ"},   {"juen", "ㄓㄨㄣ"},   {"chai", "ㄔㄞ"},
    {"chau", "ㄔㄠ"},     {"chou", "ㄔㄡ"},     {"chan", "ㄔㄢ"},
    {"chen", "ㄔㄣ"},     {"chua", "ㄔㄨㄚ"},   {"shai", "ㄕㄞ"},
    {"shei", "ㄕㄟ"},     {"shau", "ㄕㄠ"},     {"shou", "ㄕㄡ"},
    {"shan", "ㄕㄢ"},     {"shen", "ㄕㄣ"},     {"shua", "ㄕㄨㄚ"},
    {"shuo", "ㄕㄨㄛ"},   {"rang", "ㄖㄤ"},     {"reng", "ㄖㄥ"},
    {"ruan", "ㄖㄨㄢ"},   {"rung", "ㄖㄨㄥ"},   {"sang", "ㄙㄤ"},
    {"seng", "ㄙㄥ"},     {"suan", "ㄙㄨㄢ"},   {"sung", "ㄙㄨㄥ"},
    {"yang", "ㄧㄤ"},     {"ying", "ㄧㄥ"},     {"wang", "ㄨㄤ"},
    {"weng", "ㄨㄥ"},     {"yuan", "ㄩㄢ"},     {"yung", "ㄩㄥ"},
    {"niue", "ㄋㄩㄝ"},   {"liue", "ㄌㄩㄝ"},   {"guei", "ㄍㄨㄟ"},
    {"kuei", "ㄎㄨㄟ"},   {"jiou", "ㄐㄧㄡ"},   {"jiun", "ㄐㄩㄣ"},
    {"chia", "ㄑㄧㄚ"},   {"chin", "ㄑㄧㄣ"},   {"shin", "ㄒㄧㄣ"},
    {"tzai", "ㄗㄞ"},     {"tzei", "ㄗㄟ"},     {"tzau", "ㄗㄠ"},
    {"tzou", "ㄗㄡ"},     {"tzan", "ㄗㄢ"},     {"tzen", "ㄗㄣ"},
    {"tsai", "ㄘㄞ"},     {"tsau", "ㄘㄠ"},     {"tsou", "ㄘㄡ"},
    {"tsan", "ㄘㄢ"},     {"tsen", "ㄘㄣ"},     {"chuo", "ㄔㄨㄛ"},
    {"miou", "ㄇㄧㄡ"},   {"diou", "ㄉㄧㄡ"},   {"duei", "ㄉㄨㄟ"},
    {"duen", "ㄉㄨㄣ"},   {"tuei", "ㄊㄨㄟ"},   {"tuen", "ㄊㄨㄣ"},
    {"niou", "ㄋㄧㄡ"},   {"nuei", "ㄋㄨㄟ"},   {"nuen", "ㄋㄨㄣ"},
    {"liou", "ㄌㄧㄡ"},   {"luen", "ㄌㄨㄣ"},   {"guen", "ㄍㄨㄣ"},
    {"kuen", "ㄎㄨㄣ"},   {"huei", "ㄏㄨㄟ"},   {"huen", "ㄏㄨㄣ"},
    {"ruei", "ㄖㄨㄟ"},   {"ruen", "ㄖㄨㄣ"},   {"tzuo", "ㄗㄨㄛ"},
    {"tsuo", "ㄘㄨㄛ"},   {"suei", "ㄙㄨㄟ"},   {"suen", "ㄙㄨㄣ"},
    {"chiu", "ㄑㄩ"},     {"giau", "ㄍㄧㄠ"},   {"shie", "ㄒㄧㄝ"},
    {"shia", "ㄒㄧㄚ"},   {"shiu", "ㄒㄩ"},     {"jie", "ㄐㄧㄝ"},
    {"jai", "ㄓㄞ"},      {"jei", "ㄓㄟ"},      {"jau", "ㄓㄠ"},
    {"jou", "ㄓㄡ"},      {"jan", "ㄓㄢ"},      {"jen", "ㄓㄣ"},
    {"jua", "ㄓㄨㄚ"},    {"bie", "ㄅㄧㄝ"},    {"pie", "ㄆㄧㄝ"},
    {"mie", "ㄇㄧㄝ"},    {"die", "ㄉㄧㄝ"},    {"tie", "ㄊㄧㄝ"},
    {"nie", "ㄋㄧㄝ"},    {"lie", "ㄌㄧㄝ"},    {"jia", "ㄐㄧㄚ"},
    {"jin", "ㄐㄧㄣ"},    {"chr", "ㄔ"},        {"shr", "ㄕ"},
    {"yue", "ㄩㄝ"},      {"juo", "ㄓㄨㄛ"},    {"bai", "ㄅㄞ"},
    {"bei", "ㄅㄟ"},      {"bau", "ㄅㄠ"},      {"ban", "ㄅㄢ"},
    {"ben", "ㄅㄣ"},      {"bin", "ㄅㄧㄣ"},    {"pai", "ㄆㄞ"},
    {"pei", "ㄆㄟ"},      {"pau", "ㄆㄠ"},      {"pou", "ㄆㄡ"},
    {"pan", "ㄆㄢ"},      {"pen", "ㄆㄣ"},      {"pia", "ㄆㄧㄚ"},
    {"pin", "ㄆㄧㄣ"},    {"mai", "ㄇㄞ"},      {"mei", "ㄇㄟ"},
    {"mau", "ㄇㄠ"},      {"mou", "ㄇㄡ"},      {"man", "ㄇㄢ"},
    {"men", "ㄇㄣ"},      {"min", "ㄇㄧㄣ"},    {"fei", "ㄈㄟ"},
    {"fou", "ㄈㄡ"},      {"fan", "ㄈㄢ"},      {"fen", "ㄈㄣ"},
    {"dai", "ㄉㄞ"},      {"dei", "ㄉㄟ"},      {"dau", "ㄉㄠ"},
    {"dou", "ㄉㄡ"},      {"dan", "ㄉㄢ"},      {"den", "ㄉㄣ"},
    {"dia", "ㄉㄧㄚ"},    {"tai", "ㄊㄞ"},      {"tau", "ㄊㄠ"},
    {"tou", "ㄊㄡ"},      {"tan", "ㄊㄢ"},      {"nai", "ㄋㄞ"},
    {"nei", "ㄋㄟ"},      {"nau", "ㄋㄠ"},      {"nou", "ㄋㄡ"},
    {"nan", "ㄋㄢ"},      {"nen", "ㄋㄣ"},      {"nin", "ㄋㄧㄣ"},
    {"lai", "ㄌㄞ"},      {"lei", "ㄌㄟ"},      {"lau", "ㄌㄠ"},
    {"lou", "ㄌㄡ"},      {"lan", "ㄌㄢ"},      {"lia", "ㄌㄧㄚ"},
    {"lin", "ㄌㄧㄣ"},    {"gai", "ㄍㄞ"},      {"gei", "ㄍㄟ"},
    {"gau", "ㄍㄠ"},      {"gou", "ㄍㄡ"},      {"gan", "ㄍㄢ"},
    {"gen", "ㄍㄣ"},      {"gua", "ㄍㄨㄚ"},    {"guo", "ㄍㄨㄛ"},
    {"gue", "ㄍㄨㄜ"},    {"kai", "ㄎㄞ"},      {"kau", "ㄎㄠ"},
    {"kou", "ㄎㄡ"},      {"kan", "ㄎㄢ"},      {"ken", "ㄎㄣ"},
    {"kua", "ㄎㄨㄚ"},    {"kuo", "ㄎㄨㄛ"},    {"hai", "ㄏㄞ"},
    {"hei", "ㄏㄟ"},      {"hau", "ㄏㄠ"},      {"hou", "ㄏㄡ"},
    {"han", "ㄏㄢ"},      {"hen", "ㄏㄣ"},      {"hua", "ㄏㄨㄚ"},
    {"huo", "ㄏㄨㄛ"},    {"cha", "ㄔㄚ"},      {"che", "ㄔㄜ"},
    {"chu", "ㄔㄨ"},      {"sha", "ㄕㄚ"},      {"she", "ㄕㄜ"},
    {"shu", "ㄕㄨ"},      {"rau", "ㄖㄠ"},      {"rou", "ㄖㄡ"},
    {"ran", "ㄖㄢ"},      {"ren", "ㄖㄣ"},      {"sai", "ㄙㄞ"},
    {"sei", "ㄙㄟ"},      {"sau", "ㄙㄠ"},      {"sou", "ㄙㄡ"},
    {"san", "ㄙㄢ"},      {"sen", "ㄙㄣ"},      {"ang", "ㄤ"},
    {"eng", "ㄥ"},        {"yai", "ㄧㄞ"},      {"yau", "ㄧㄠ"},
    {"yan", "ㄧㄢ"},      {"yin", "ㄧㄣ"},      {"wai", "ㄨㄞ"},
    {"wei", "ㄨㄟ"},      {"wan", "ㄨㄢ"},      {"wen", "ㄨㄣ"},
    {"yun", "ㄩㄣ"},      {"jiu", "ㄐㄩ"},      {"chi", "ㄑㄧ"},
    {"shi", "ㄒㄧ"},      {"tza", "ㄗㄚ"},      {"tze", "ㄗㄜ"},
    {"tzu", "ㄗㄨ"},      {"tsz", "ㄘ"},        {"tsa", "ㄘㄚ"},
    {"tse", "ㄘㄜ"},      {"tsu", "ㄘㄨ"},      {"duo", "ㄉㄨㄛ"},
    {"tuo", "ㄊㄨㄛ"},    {"nuo", "ㄋㄨㄛ"},    {"luo", "ㄌㄨㄛ"},
    {"ruo", "ㄖㄨㄛ"},    {"suo", "ㄙㄨㄛ"},    {"you", "ㄧㄡ"},
    {"niu", "ㄋㄩ"},      {"liu", "ㄌㄩ"},      {"gin", "ㄍㄧㄣ"},
    {"bo", "ㄅㄛ"},       {"po", "ㄆㄛ"},       {"mo", "ㄇㄛ"},
    {"fo", "ㄈㄛ"},       {"jr", "ㄓ"},         {"ja", "ㄓㄚ"},
    {"je", "ㄓㄜ"},       {"ju", "ㄓㄨ"},       {"ji", "ㄐㄧ"},
    {"tz", "ㄗ"},         {"sz", "ㄙ"},         {"er", "ㄦ"},
    {"ye", "ㄧㄝ"},       {"ba", "ㄅㄚ"},       {"bi", "ㄅㄧ"},
    {"bu", "ㄅㄨ"},       {"pa", "ㄆㄚ"},       {"pi", "ㄆㄧ"},
    {"pu", "ㄆㄨ"},       {"ma", "ㄇㄚ"},       {"me", "ㄇㄜ"},
    {"mi", "ㄇㄧ"},       {"mu", "ㄇㄨ"},       {"fa", "ㄈㄚ"},
    {"fu", "ㄈㄨ"},       {"da", "ㄉㄚ"},       {"de", "ㄉㄜ"},
    {"di", "ㄉㄧ"},       {"du", "ㄉㄨ"},       {"ta", "ㄊㄚ"},
    {"te", "ㄊㄜ"},       {"ti", "ㄊㄧ"},       {"tu", "ㄊㄨ"},
    {"na", "ㄋㄚ"},       {"ne", "ㄋㄜ"},       {"ni", "ㄋㄧ"},
    {"nu", "ㄋㄨ"},       {"la", "ㄌㄚ"},       {"lo", "ㄌㄛ"},
    {"le", "ㄌㄜ"},       {"li", "ㄌㄧ"},       {"lu", "ㄌㄨ"},
    {"ga", "ㄍㄚ"},       {"ge", "ㄍㄜ"},       {"gu", "ㄍㄨ"},
    {"ka", "ㄎㄚ"},       {"ke", "ㄎㄜ"},       {"ku", "ㄎㄨ"},
    {"ha", "ㄏㄚ"},       {"he", "ㄏㄜ"},       {"hu", "ㄏㄨ"},
    {"re", "ㄖㄜ"},       {"ru", "ㄖㄨ"},       {"sa", "ㄙㄚ"},
    {"se", "ㄙㄜ"},       {"su", "ㄙㄨ"},       {"eh", "ㄝ"},
    {"ai", "ㄞ"},         {"ei", "ㄟ"},         {"au", "ㄠ"},
    {"ou", "ㄡ"},         {"an", "ㄢ"},         {"en", "ㄣ"},
    {"ya", "ㄧㄚ"},       {"yo", "ㄧㄛ"},       {"wu", "ㄨ"},
    {"wa", "ㄨㄚ"},       {"wo", "ㄨㄛ"},       {"yu", "ㄩ"},
    {"ch", "ㄑ"},         {"yi", "ㄧ"},         {"r", "ㄖ"},
    {"a", "ㄚ"},          {"o", "ㄛ"},          {"e", "ㄜ"}};

/// 耶魯拼音排列專用處理陣列
inline static std::map<std::string, std::string> mapYalePinyin = {
    {"chwang", "ㄔㄨㄤ"}, {"shwang", "ㄕㄨㄤ"}, {"chyang", "ㄑㄧㄤ"},
    {"chyung", "ㄑㄩㄥ"}, {"chywan", "ㄑㄩㄢ"}, {"byang", "ㄅㄧㄤ"},
    {"dwang", "ㄉㄨㄤ"},  {"jwang", "ㄓㄨㄤ"},  {"syang", "ㄒㄧㄤ"},
    {"syung", "ㄒㄩㄥ"},  {"jyang", "ㄐㄧㄤ"},  {"jyung", "ㄐㄩㄥ"},
    {"nyang", "ㄋㄧㄤ"},  {"lyang", "ㄌㄧㄤ"},  {"gwang", "ㄍㄨㄤ"},
    {"kwang", "ㄎㄨㄤ"},  {"hwang", "ㄏㄨㄤ"},  {"chang", "ㄔㄤ"},
    {"cheng", "ㄔㄥ"},    {"chwai", "ㄔㄨㄞ"},  {"chwan", "ㄔㄨㄢ"},
    {"chung", "ㄔㄨㄥ"},  {"shang", "ㄕㄤ"},    {"sheng", "ㄕㄥ"},
    {"shwai", "ㄕㄨㄞ"},  {"shwan", "ㄕㄨㄢ"},  {"sywan", "ㄒㄩㄢ"},
    {"jywan", "ㄐㄩㄢ"},  {"chyau", "ㄑㄧㄠ"},  {"chyan", "ㄑㄧㄢ"},
    {"ching", "ㄑㄧㄥ"},  {"sying", "ㄒㄧㄥ"},  {"dzang", "ㄗㄤ"},
    {"dzeng", "ㄗㄥ"},    {"dzwan", "ㄗㄨㄢ"},  {"dzung", "ㄗㄨㄥ"},
    {"tsang", "ㄘㄤ"},    {"tseng", "ㄘㄥ"},    {"tswan", "ㄘㄨㄢ"},
    {"tsung", "ㄘㄨㄥ"},  {"chywe", "ㄑㄩㄝ"},  {"lywan", "ㄌㄩㄢ"},
    {"chwei", "ㄔㄨㄟ"},  {"chwun", "ㄔㄨㄣ"},  {"shwei", "ㄕㄨㄟ"},
    {"shwun", "ㄕㄨㄣ"},  {"chyou", "ㄑㄧㄡ"},  {"chyun", "ㄑㄩㄣ"},
    {"dzwei", "ㄗㄨㄟ"},  {"dzwun", "ㄗㄨㄣ"},  {"tswei", "ㄘㄨㄟ"},
    {"tswun", "ㄘㄨㄣ"},  {"kyang", "ㄎㄧㄤ"},  {"jang", "ㄓㄤ"},
    {"jeng", "ㄓㄥ"},     {"jwai", "ㄓㄨㄞ"},   {"jwan", "ㄓㄨㄢ"},
    {"jung", "ㄓㄨㄥ"},   {"syau", "ㄒㄧㄠ"},   {"syan", "ㄒㄧㄢ"},
    {"jyau", "ㄐㄧㄠ"},   {"jyan", "ㄐㄧㄢ"},   {"jing", "ㄐㄧㄥ"},
    {"sywe", "ㄒㄩㄝ"},   {"jywe", "ㄐㄩㄝ"},   {"chye", "ㄑㄧㄝ"},
    {"bang", "ㄅㄤ"},     {"beng", "ㄅㄥ"},     {"byau", "ㄅㄧㄠ"},
    {"byan", "ㄅㄧㄢ"},   {"bing", "ㄅㄧㄥ"},   {"pang", "ㄆㄤ"},
    {"peng", "ㄆㄥ"},     {"pyau", "ㄆㄧㄠ"},   {"pyan", "ㄆㄧㄢ"},
    {"ping", "ㄆㄧㄥ"},   {"mang", "ㄇㄤ"},     {"meng", "ㄇㄥ"},
    {"myau", "ㄇㄧㄠ"},   {"myan", "ㄇㄧㄢ"},   {"ming", "ㄇㄧㄥ"},
    {"fang", "ㄈㄤ"},     {"feng", "ㄈㄥ"},     {"fyau", "ㄈㄧㄠ"},
    {"dang", "ㄉㄤ"},     {"deng", "ㄉㄥ"},     {"dyau", "ㄉㄧㄠ"},
    {"dyan", "ㄉㄧㄢ"},   {"ding", "ㄉㄧㄥ"},   {"dwan", "ㄉㄨㄢ"},
    {"dung", "ㄉㄨㄥ"},   {"tang", "ㄊㄤ"},     {"teng", "ㄊㄥ"},
    {"tyau", "ㄊㄧㄠ"},   {"tyan", "ㄊㄧㄢ"},   {"ting", "ㄊㄧㄥ"},
    {"twan", "ㄊㄨㄢ"},   {"tung", "ㄊㄨㄥ"},   {"nang", "ㄋㄤ"},
    {"neng", "ㄋㄥ"},     {"nyau", "ㄋㄧㄠ"},   {"nyan", "ㄋㄧㄢ"},
    {"ning", "ㄋㄧㄥ"},   {"nwan", "ㄋㄨㄢ"},   {"nung", "ㄋㄨㄥ"},
    {"lang", "ㄌㄤ"},     {"leng", "ㄌㄥ"},     {"lyau", "ㄌㄧㄠ"},
    {"lyan", "ㄌㄧㄢ"},   {"ling", "ㄌㄧㄥ"},   {"lwan", "ㄌㄨㄢ"},
    {"lung", "ㄌㄨㄥ"},   {"gang", "ㄍㄤ"},     {"geng", "ㄍㄥ"},
    {"gwai", "ㄍㄨㄞ"},   {"gwan", "ㄍㄨㄢ"},   {"gung", "ㄍㄨㄥ"},
    {"kang", "ㄎㄤ"},     {"keng", "ㄎㄥ"},     {"kwai", "ㄎㄨㄞ"},
    {"kwan", "ㄎㄨㄢ"},   {"kung", "ㄎㄨㄥ"},   {"hang", "ㄏㄤ"},
    {"heng", "ㄏㄥ"},     {"hwai", "ㄏㄨㄞ"},   {"hwan", "ㄏㄨㄢ"},
    {"hung", "ㄏㄨㄥ"},   {"jwei", "ㄓㄨㄟ"},   {"jwun", "ㄓㄨㄣ"},
    {"chai", "ㄔㄞ"},     {"chau", "ㄔㄠ"},     {"chou", "ㄔㄡ"},
    {"chan", "ㄔㄢ"},     {"chen", "ㄔㄣ"},     {"chwa", "ㄔㄨㄚ"},
    {"shai", "ㄕㄞ"},     {"shei", "ㄕㄟ"},     {"shau", "ㄕㄠ"},
    {"shou", "ㄕㄡ"},     {"shan", "ㄕㄢ"},     {"shen", "ㄕㄣ"},
    {"shwa", "ㄕㄨㄚ"},   {"shwo", "ㄕㄨㄛ"},   {"rang", "ㄖㄤ"},
    {"reng", "ㄖㄥ"},     {"rwan", "ㄖㄨㄢ"},   {"rung", "ㄖㄨㄥ"},
    {"sang", "ㄙㄤ"},     {"seng", "ㄙㄥ"},     {"swan", "ㄙㄨㄢ"},
    {"sung", "ㄙㄨㄥ"},   {"yang", "ㄧㄤ"},     {"ying", "ㄧㄥ"},
    {"wang", "ㄨㄤ"},     {"weng", "ㄨㄥ"},     {"ywan", "ㄩㄢ"},
    {"yung", "ㄩㄥ"},     {"syou", "ㄒㄧㄡ"},   {"syun", "ㄒㄩㄣ"},
    {"nywe", "ㄋㄩㄝ"},   {"lywe", "ㄌㄩㄝ"},   {"gwei", "ㄍㄨㄟ"},
    {"kwei", "ㄎㄨㄟ"},   {"jyou", "ㄐㄧㄡ"},   {"jyun", "ㄐㄩㄣ"},
    {"chya", "ㄑㄧㄚ"},   {"chin", "ㄑㄧㄣ"},   {"syin", "ㄒㄧㄣ"},
    {"dzai", "ㄗㄞ"},     {"dzei", "ㄗㄟ"},     {"dzau", "ㄗㄠ"},
    {"dzou", "ㄗㄡ"},     {"dzan", "ㄗㄢ"},     {"dzen", "ㄗㄣ"},
    {"tsai", "ㄘㄞ"},     {"tsau", "ㄘㄠ"},     {"tsou", "ㄘㄡ"},
    {"tsan", "ㄘㄢ"},     {"tsen", "ㄘㄣ"},     {"chwo", "ㄔㄨㄛ"},
    {"myou", "ㄇㄧㄡ"},   {"dyou", "ㄉㄧㄡ"},   {"dwei", "ㄉㄨㄟ"},
    {"dwun", "ㄉㄨㄣ"},   {"twei", "ㄊㄨㄟ"},   {"twun", "ㄊㄨㄣ"},
    {"nyou", "ㄋㄧㄡ"},   {"nwei", "ㄋㄨㄟ"},   {"nwun", "ㄋㄨㄣ"},
    {"lyou", "ㄌㄧㄡ"},   {"lwun", "ㄌㄨㄣ"},   {"gwun", "ㄍㄨㄣ"},
    {"kwun", "ㄎㄨㄣ"},   {"hwei", "ㄏㄨㄟ"},   {"hwun", "ㄏㄨㄣ"},
    {"rwei", "ㄖㄨㄟ"},   {"rwun", "ㄖㄨㄣ"},   {"dzwo", "ㄗㄨㄛ"},
    {"tswo", "ㄘㄨㄛ"},   {"swei", "ㄙㄨㄟ"},   {"swun", "ㄙㄨㄣ"},
    {"chyu", "ㄑㄩ"},     {"giau", "ㄍㄧㄠ"},   {"sye", "ㄒㄧㄝ"},
    {"jye", "ㄐㄧㄝ"},    {"jai", "ㄓㄞ"},      {"jei", "ㄓㄟ"},
    {"jau", "ㄓㄠ"},      {"jou", "ㄓㄡ"},      {"jan", "ㄓㄢ"},
    {"jen", "ㄓㄣ"},      {"jwa", "ㄓㄨㄚ"},    {"sya", "ㄒㄧㄚ"},
    {"bye", "ㄅㄧㄝ"},    {"pye", "ㄆㄧㄝ"},    {"mye", "ㄇㄧㄝ"},
    {"dye", "ㄉㄧㄝ"},    {"tye", "ㄊㄧㄝ"},    {"nye", "ㄋㄧㄝ"},
    {"lye", "ㄌㄧㄝ"},    {"jya", "ㄐㄧㄚ"},    {"jin", "ㄐㄧㄣ"},
    {"chr", "ㄔ"},        {"shr", "ㄕ"},        {"ywe", "ㄩㄝ"},
    {"jwo", "ㄓㄨㄛ"},    {"bai", "ㄅㄞ"},      {"bei", "ㄅㄟ"},
    {"bau", "ㄅㄠ"},      {"ban", "ㄅㄢ"},      {"ben", "ㄅㄣ"},
    {"bin", "ㄅㄧㄣ"},    {"pai", "ㄆㄞ"},      {"pei", "ㄆㄟ"},
    {"pau", "ㄆㄠ"},      {"pou", "ㄆㄡ"},      {"pan", "ㄆㄢ"},
    {"pen", "ㄆㄣ"},      {"pya", "ㄆㄧㄚ"},    {"pin", "ㄆㄧㄣ"},
    {"mai", "ㄇㄞ"},      {"mei", "ㄇㄟ"},      {"mau", "ㄇㄠ"},
    {"mou", "ㄇㄡ"},      {"man", "ㄇㄢ"},      {"men", "ㄇㄣ"},
    {"min", "ㄇㄧㄣ"},    {"fei", "ㄈㄟ"},      {"fou", "ㄈㄡ"},
    {"fan", "ㄈㄢ"},      {"fen", "ㄈㄣ"},      {"dai", "ㄉㄞ"},
    {"dei", "ㄉㄟ"},      {"dau", "ㄉㄠ"},      {"dou", "ㄉㄡ"},
    {"dan", "ㄉㄢ"},      {"den", "ㄉㄣ"},      {"dya", "ㄉㄧㄚ"},
    {"tai", "ㄊㄞ"},      {"tau", "ㄊㄠ"},      {"tou", "ㄊㄡ"},
    {"tan", "ㄊㄢ"},      {"nai", "ㄋㄞ"},      {"nei", "ㄋㄟ"},
    {"nau", "ㄋㄠ"},      {"nou", "ㄋㄡ"},      {"nan", "ㄋㄢ"},
    {"nen", "ㄋㄣ"},      {"nin", "ㄋㄧㄣ"},    {"lai", "ㄌㄞ"},
    {"lei", "ㄌㄟ"},      {"lau", "ㄌㄠ"},      {"lou", "ㄌㄡ"},
    {"lan", "ㄌㄢ"},      {"lya", "ㄌㄧㄚ"},    {"lin", "ㄌㄧㄣ"},
    {"gai", "ㄍㄞ"},      {"gei", "ㄍㄟ"},      {"gau", "ㄍㄠ"},
    {"gou", "ㄍㄡ"},      {"gan", "ㄍㄢ"},      {"gen", "ㄍㄣ"},
    {"gwa", "ㄍㄨㄚ"},    {"gwo", "ㄍㄨㄛ"},    {"gue", "ㄍㄨㄜ"},
    {"kai", "ㄎㄞ"},      {"kau", "ㄎㄠ"},      {"kou", "ㄎㄡ"},
    {"kan", "ㄎㄢ"},      {"ken", "ㄎㄣ"},      {"kwa", "ㄎㄨㄚ"},
    {"kwo", "ㄎㄨㄛ"},    {"hai", "ㄏㄞ"},      {"hei", "ㄏㄟ"},
    {"hau", "ㄏㄠ"},      {"hou", "ㄏㄡ"},      {"han", "ㄏㄢ"},
    {"hen", "ㄏㄣ"},      {"hwa", "ㄏㄨㄚ"},    {"hwo", "ㄏㄨㄛ"},
    {"cha", "ㄔㄚ"},      {"che", "ㄔㄜ"},      {"chu", "ㄔㄨ"},
    {"sha", "ㄕㄚ"},      {"she", "ㄕㄜ"},      {"shu", "ㄕㄨ"},
    {"rau", "ㄖㄠ"},      {"rou", "ㄖㄡ"},      {"ran", "ㄖㄢ"},
    {"ren", "ㄖㄣ"},      {"sai", "ㄙㄞ"},      {"sei", "ㄙㄟ"},
    {"sau", "ㄙㄠ"},      {"sou", "ㄙㄡ"},      {"san", "ㄙㄢ"},
    {"sen", "ㄙㄣ"},      {"ang", "ㄤ"},        {"eng", "ㄥ"},
    {"yai", "ㄧㄞ"},      {"yau", "ㄧㄠ"},      {"yan", "ㄧㄢ"},
    {"yin", "ㄧㄣ"},      {"wai", "ㄨㄞ"},      {"wei", "ㄨㄟ"},
    {"wan", "ㄨㄢ"},      {"wen", "ㄨㄣ"},      {"yun", "ㄩㄣ"},
    {"syu", "ㄒㄩ"},      {"jyu", "ㄐㄩ"},      {"chi", "ㄑㄧ"},
    {"syi", "ㄒㄧ"},      {"dza", "ㄗㄚ"},      {"dze", "ㄗㄜ"},
    {"dzu", "ㄗㄨ"},      {"tsz", "ㄘ"},        {"tsa", "ㄘㄚ"},
    {"tse", "ㄘㄜ"},      {"tsu", "ㄘㄨ"},      {"dwo", "ㄉㄨㄛ"},
    {"two", "ㄊㄨㄛ"},    {"nwo", "ㄋㄨㄛ"},    {"lwo", "ㄌㄨㄛ"},
    {"rwo", "ㄖㄨㄛ"},    {"swo", "ㄙㄨㄛ"},    {"you", "ㄧㄡ"},
    {"nyu", "ㄋㄩ"},      {"lyu", "ㄌㄩ"},      {"bwo", "ㄅㄛ"},
    {"pwo", "ㄆㄛ"},      {"mwo", "ㄇㄛ"},      {"fwo", "ㄈㄛ"},
    {"gin", "ㄍㄧㄣ"},    {"jr", "ㄓ"},         {"ja", "ㄓㄚ"},
    {"je", "ㄓㄜ"},       {"ju", "ㄓㄨ"},       {"ji", "ㄐㄧ"},
    {"dz", "ㄗ"},         {"sz", "ㄙ"},         {"er", "ㄦ"},
    {"ye", "ㄧㄝ"},       {"ba", "ㄅㄚ"},       {"bi", "ㄅㄧ"},
    {"bu", "ㄅㄨ"},       {"pa", "ㄆㄚ"},       {"pi", "ㄆㄧ"},
    {"pu", "ㄆㄨ"},       {"ma", "ㄇㄚ"},       {"me", "ㄇㄜ"},
    {"mi", "ㄇㄧ"},       {"mu", "ㄇㄨ"},       {"fa", "ㄈㄚ"},
    {"fu", "ㄈㄨ"},       {"da", "ㄉㄚ"},       {"de", "ㄉㄜ"},
    {"di", "ㄉㄧ"},       {"du", "ㄉㄨ"},       {"ta", "ㄊㄚ"},
    {"te", "ㄊㄜ"},       {"ti", "ㄊㄧ"},       {"tu", "ㄊㄨ"},
    {"na", "ㄋㄚ"},       {"ne", "ㄋㄜ"},       {"ni", "ㄋㄧ"},
    {"nu", "ㄋㄨ"},       {"la", "ㄌㄚ"},       {"lo", "ㄌㄛ"},
    {"le", "ㄌㄜ"},       {"li", "ㄌㄧ"},       {"lu", "ㄌㄨ"},
    {"ga", "ㄍㄚ"},       {"ge", "ㄍㄜ"},       {"gu", "ㄍㄨ"},
    {"ka", "ㄎㄚ"},       {"ke", "ㄎㄜ"},       {"ku", "ㄎㄨ"},
    {"ha", "ㄏㄚ"},       {"he", "ㄏㄜ"},       {"hu", "ㄏㄨ"},
    {"re", "ㄖㄜ"},       {"ru", "ㄖㄨ"},       {"sa", "ㄙㄚ"},
    {"se", "ㄙㄜ"},       {"su", "ㄙㄨ"},       {"eh", "ㄝ"},
    {"ai", "ㄞ"},         {"ei", "ㄟ"},         {"au", "ㄠ"},
    {"ou", "ㄡ"},         {"an", "ㄢ"},         {"en", "ㄣ"},
    {"ya", "ㄧㄚ"},       {"yo", "ㄧㄛ"},       {"wu", "ㄨ"},
    {"wa", "ㄨㄚ"},       {"wo", "ㄨㄛ"},       {"yu", "ㄩ"},
    {"ch", "ㄑ"},         {"yi", "ㄧ"},         {"r", "ㄖ"},
    {"a", "ㄚ"},          {"o", "ㄛ"},          {"e", "ㄜ"}};

/// 華羅拼音排列專用處理陣列
inline static std::map<std::string, std::string> mapHualuoPinyin = {
    {"shuang", "ㄕㄨㄤ"}, {"jhuang", "ㄓㄨㄤ"}, {"chyueh", "ㄑㄩㄝ"},
    {"chyuan", "ㄑㄩㄢ"}, {"chyong", "ㄑㄩㄥ"}, {"chiang", "ㄑㄧㄤ"},
    {"chuang", "ㄔㄨㄤ"}, {"biang", "ㄅㄧㄤ"},  {"duang", "ㄉㄨㄤ"},
    {"kyang", "ㄎㄧㄤ"},  {"syueh", "ㄒㄩㄝ"},  {"syuan", "ㄒㄩㄢ"},
    {"syong", "ㄒㄩㄥ"},  {"sihei", "ㄙㄟ"},    {"siang", "ㄒㄧㄤ"},
    {"shuei", "ㄕㄨㄟ"},  {"shuan", "ㄕㄨㄢ"},  {"shuai", "ㄕㄨㄞ"},
    {"sheng", "ㄕㄥ"},    {"shang", "ㄕㄤ"},    {"nyueh", "ㄋㄩㄝ"},
    {"niang", "ㄋㄧㄤ"},  {"lyueh", "ㄌㄩㄝ"},  {"lyuan", "ㄌㄩㄢ"},
    {"liang", "ㄌㄧㄤ"},  {"kuang", "ㄎㄨㄤ"},  {"jyueh", "ㄐㄩㄝ"},
    {"jyuan", "ㄐㄩㄢ"},  {"jyong", "ㄐㄩㄥ"},  {"jiang", "ㄐㄧㄤ"},
    {"jhuei", "ㄓㄨㄟ"},  {"jhuan", "ㄓㄨㄢ"},  {"jhuai", "ㄓㄨㄞ"},
    {"jhong", "ㄓㄨㄥ"},  {"jheng", "ㄓㄥ"},    {"jhang", "ㄓㄤ"},
    {"huang", "ㄏㄨㄤ"},  {"guang", "ㄍㄨㄤ"},  {"chyun", "ㄑㄩㄣ"},
    {"tsuei", "ㄘㄨㄟ"},  {"tsuan", "ㄘㄨㄢ"},  {"tsong", "ㄘㄨㄥ"},
    {"chiou", "ㄑㄧㄡ"},  {"ching", "ㄑㄧㄥ"},  {"chieh", "ㄑㄧㄝ"},
    {"chiao", "ㄑㄧㄠ"},  {"chian", "ㄑㄧㄢ"},  {"chuei", "ㄔㄨㄟ"},
    {"chuan", "ㄔㄨㄢ"},  {"chuai", "ㄔㄨㄞ"},  {"chong", "ㄔㄨㄥ"},
    {"cheng", "ㄔㄥ"},    {"chang", "ㄔㄤ"},    {"tseng", "ㄘㄥ"},
    {"tsang", "ㄘㄤ"},    {"gyao", "ㄍㄧㄠ"},   {"fiao", "ㄈㄧㄠ"},
    {"zuei", "ㄗㄨㄟ"},   {"zuan", "ㄗㄨㄢ"},   {"zong", "ㄗㄨㄥ"},
    {"zeng", "ㄗㄥ"},     {"zang", "ㄗㄤ"},     {"yueh", "ㄩㄝ"},
    {"yuan", "ㄩㄢ"},     {"yong", "ㄩㄥ"},     {"ying", "ㄧㄥ"},
    {"yang", "ㄧㄤ"},     {"wong", "ㄨㄥ"},     {"wang", "ㄨㄤ"},
    {"tuei", "ㄊㄨㄟ"},   {"tuan", "ㄊㄨㄢ"},   {"tong", "ㄊㄨㄥ"},
    {"ting", "ㄊㄧㄥ"},   {"tieh", "ㄊㄧㄝ"},   {"tiao", "ㄊㄧㄠ"},
    {"tian", "ㄊㄧㄢ"},   {"teng", "ㄊㄥ"},     {"tang", "ㄊㄤ"},
    {"syun", "ㄒㄩㄣ"},   {"suei", "ㄙㄨㄟ"},   {"suan", "ㄙㄨㄢ"},
    {"song", "ㄙㄨㄥ"},   {"siou", "ㄒㄧㄡ"},   {"sing", "ㄒㄧㄥ"},
    {"sieh", "ㄒㄧㄝ"},   {"siao", "ㄒㄧㄠ"},   {"sian", "ㄒㄧㄢ"},
    {"shuo", "ㄕㄨㄛ"},   {"shun", "ㄕㄨㄣ"},   {"shua", "ㄕㄨㄚ"},
    {"shou", "ㄕㄡ"},     {"shih", "ㄕ"},       {"shen", "ㄕㄣ"},
    {"shei", "ㄕㄟ"},     {"shao", "ㄕㄠ"},     {"shan", "ㄕㄢ"},
    {"shai", "ㄕㄞ"},     {"seng", "ㄙㄥ"},     {"sang", "ㄙㄤ"},
    {"ruei", "ㄖㄨㄟ"},   {"ruan", "ㄖㄨㄢ"},   {"rong", "ㄖㄨㄥ"},
    {"reng", "ㄖㄥ"},     {"rang", "ㄖㄤ"},     {"ping", "ㄆㄧㄥ"},
    {"pieh", "ㄆㄧㄝ"},   {"piao", "ㄆㄧㄠ"},   {"pian", "ㄆㄧㄢ"},
    {"peng", "ㄆㄥ"},     {"pang", "ㄆㄤ"},     {"nuei", "ㄋㄨㄟ"},
    {"nuan", "ㄋㄨㄢ"},   {"nong", "ㄋㄨㄥ"},   {"niou", "ㄋㄧㄡ"},
    {"ning", "ㄋㄧㄥ"},   {"nieh", "ㄋㄧㄝ"},   {"niao", "ㄋㄧㄠ"},
    {"nian", "ㄋㄧㄢ"},   {"neng", "ㄋㄥ"},     {"nang", "ㄋㄤ"},
    {"miou", "ㄇㄧㄡ"},   {"ming", "ㄇㄧㄥ"},   {"mieh", "ㄇㄧㄝ"},
    {"miao", "ㄇㄧㄠ"},   {"mian", "ㄇㄧㄢ"},   {"meng", "ㄇㄥ"},
    {"mang", "ㄇㄤ"},     {"luan", "ㄌㄨㄢ"},   {"long", "ㄌㄨㄥ"},
    {"liou", "ㄌㄧㄡ"},   {"ling", "ㄌㄧㄥ"},   {"lieh", "ㄌㄧㄝ"},
    {"liao", "ㄌㄧㄠ"},   {"lian", "ㄌㄧㄢ"},   {"leng", "ㄌㄥ"},
    {"lang", "ㄌㄤ"},     {"kuei", "ㄎㄨㄟ"},   {"kuan", "ㄎㄨㄢ"},
    {"kuai", "ㄎㄨㄞ"},   {"kong", "ㄎㄨㄥ"},   {"keng", "ㄎㄥ"},
    {"kang", "ㄎㄤ"},     {"jyun", "ㄐㄩㄣ"},   {"jiou", "ㄐㄧㄡ"},
    {"jing", "ㄐㄧㄥ"},   {"jieh", "ㄐㄧㄝ"},   {"jiao", "ㄐㄧㄠ"},
    {"jian", "ㄐㄧㄢ"},   {"jhuo", "ㄓㄨㄛ"},   {"jhun", "ㄓㄨㄣ"},
    {"jhua", "ㄓㄨㄚ"},   {"jhou", "ㄓㄡ"},     {"jhih", "ㄓ"},
    {"jhen", "ㄓㄣ"},     {"jhei", "ㄓㄟ"},     {"jhao", "ㄓㄠ"},
    {"jhan", "ㄓㄢ"},     {"jhai", "ㄓㄞ"},     {"huei", "ㄏㄨㄟ"},
    {"huan", "ㄏㄨㄢ"},   {"huai", "ㄏㄨㄞ"},   {"hong", "ㄏㄨㄥ"},
    {"heng", "ㄏㄥ"},     {"hang", "ㄏㄤ"},     {"guei", "ㄍㄨㄟ"},
    {"guan", "ㄍㄨㄢ"},   {"guai", "ㄍㄨㄞ"},   {"gong", "ㄍㄨㄥ"},
    {"geng", "ㄍㄥ"},     {"gang", "ㄍㄤ"},     {"feng", "ㄈㄥ"},
    {"fang", "ㄈㄤ"},     {"duei", "ㄉㄨㄟ"},   {"duan", "ㄉㄨㄢ"},
    {"dong", "ㄉㄨㄥ"},   {"diou", "ㄉㄧㄡ"},   {"ding", "ㄉㄧㄥ"},
    {"dieh", "ㄉㄧㄝ"},   {"diao", "ㄉㄧㄠ"},   {"dian", "ㄉㄧㄢ"},
    {"deng", "ㄉㄥ"},     {"dang", "ㄉㄤ"},     {"chyu", "ㄑㄩ"},
    {"tsuo", "ㄘㄨㄛ"},   {"tsun", "ㄘㄨㄣ"},   {"tsou", "ㄘㄡ"},
    {"chin", "ㄑㄧㄣ"},   {"tsih", "ㄘ"},       {"chia", "ㄑㄧㄚ"},
    {"chuo", "ㄔㄨㄛ"},   {"chun", "ㄔㄨㄣ"},   {"chua", "ㄔㄨㄚ"},
    {"chou", "ㄔㄡ"},     {"chih", "ㄔ"},       {"chen", "ㄔㄣ"},
    {"chao", "ㄔㄠ"},     {"chan", "ㄔㄢ"},     {"chai", "ㄔㄞ"},
    {"tsen", "ㄘㄣ"},     {"tsao", "ㄘㄠ"},     {"tsan", "ㄘㄢ"},
    {"tsai", "ㄘㄞ"},     {"bing", "ㄅㄧㄥ"},   {"bieh", "ㄅㄧㄝ"},
    {"biao", "ㄅㄧㄠ"},   {"bian", "ㄅㄧㄢ"},   {"beng", "ㄅㄥ"},
    {"bang", "ㄅㄤ"},     {"gin", "ㄍㄧㄣ"},    {"den", "ㄉㄣ"},
    {"zuo", "ㄗㄨㄛ"},    {"zun", "ㄗㄨㄣ"},    {"zou", "ㄗㄡ"},
    {"zih", "ㄗ"},        {"zen", "ㄗㄣ"},      {"zei", "ㄗㄟ"},
    {"zao", "ㄗㄠ"},      {"zan", "ㄗㄢ"},      {"zai", "ㄗㄞ"},
    {"yun", "ㄩㄣ"},      {"you", "ㄧㄡ"},      {"yin", "ㄧㄣ"},
    {"yeh", "ㄧㄝ"},      {"yao", "ㄧㄠ"},      {"yan", "ㄧㄢ"},
    {"yai", "ㄧㄞ"},      {"wun", "ㄨㄣ"},      {"wei", "ㄨㄟ"},
    {"wan", "ㄨㄢ"},      {"wai", "ㄨㄞ"},      {"tuo", "ㄊㄨㄛ"},
    {"tun", "ㄊㄨㄣ"},    {"tou", "ㄊㄡ"},      {"tao", "ㄊㄠ"},
    {"tan", "ㄊㄢ"},      {"tai", "ㄊㄞ"},      {"syu", "ㄒㄩ"},
    {"suo", "ㄙㄨㄛ"},    {"sun", "ㄙㄨㄣ"},    {"sou", "ㄙㄡ"},
    {"sin", "ㄒㄧㄣ"},    {"sih", "ㄙ"},        {"sia", "ㄒㄧㄚ"},
    {"shu", "ㄕㄨ"},      {"she", "ㄕㄜ"},      {"sha", "ㄕㄚ"},
    {"sen", "ㄙㄣ"},      {"sao", "ㄙㄠ"},      {"san", "ㄙㄢ"},
    {"sai", "ㄙㄞ"},      {"ruo", "ㄖㄨㄛ"},    {"run", "ㄖㄨㄣ"},
    {"rou", "ㄖㄡ"},      {"rih", "ㄖ"},        {"ren", "ㄖㄣ"},
    {"rao", "ㄖㄠ"},      {"ran", "ㄖㄢ"},      {"pou", "ㄆㄡ"},
    {"pin", "ㄆㄧㄣ"},    {"pia", "ㄆㄧㄚ"},    {"pen", "ㄆㄣ"},
    {"pei", "ㄆㄟ"},      {"pao", "ㄆㄠ"},      {"pan", "ㄆㄢ"},
    {"pai", "ㄆㄞ"},      {"nyu", "ㄋㄩ"},      {"nuo", "ㄋㄨㄛ"},
    {"nun", "ㄋㄨㄣ"},    {"nou", "ㄋㄡ"},      {"nin", "ㄋㄧㄣ"},
    {"nen", "ㄋㄣ"},      {"nei", "ㄋㄟ"},      {"nao", "ㄋㄠ"},
    {"nan", "ㄋㄢ"},      {"nai", "ㄋㄞ"},      {"mou", "ㄇㄡ"},
    {"min", "ㄇㄧㄣ"},    {"men", "ㄇㄣ"},      {"mei", "ㄇㄟ"},
    {"mao", "ㄇㄠ"},      {"man", "ㄇㄢ"},      {"mai", "ㄇㄞ"},
    {"lyu", "ㄌㄩ"},      {"luo", "ㄌㄨㄛ"},    {"lun", "ㄌㄨㄣ"},
    {"lou", "ㄌㄡ"},      {"lin", "ㄌㄧㄣ"},    {"lia", "ㄌㄧㄚ"},
    {"lei", "ㄌㄟ"},      {"lao", "ㄌㄠ"},      {"lan", "ㄌㄢ"},
    {"lai", "ㄌㄞ"},      {"kuo", "ㄎㄨㄛ"},    {"kun", "ㄎㄨㄣ"},
    {"kua", "ㄎㄨㄚ"},    {"kou", "ㄎㄡ"},      {"ken", "ㄎㄣ"},
    {"kao", "ㄎㄠ"},      {"kan", "ㄎㄢ"},      {"kai", "ㄎㄞ"},
    {"jyu", "ㄐㄩ"},      {"jin", "ㄐㄧㄣ"},    {"jia", "ㄐㄧㄚ"},
    {"jhu", "ㄓㄨ"},      {"jhe", "ㄓㄜ"},      {"jha", "ㄓㄚ"},
    {"huo", "ㄏㄨㄛ"},    {"hun", "ㄏㄨㄣ"},    {"hua", "ㄏㄨㄚ"},
    {"hou", "ㄏㄡ"},      {"hen", "ㄏㄣ"},      {"hei", "ㄏㄟ"},
    {"hao", "ㄏㄠ"},      {"han", "ㄏㄢ"},      {"hai", "ㄏㄞ"},
    {"guo", "ㄍㄨㄛ"},    {"gun", "ㄍㄨㄣ"},    {"gue", "ㄍㄨㄜ"},
    {"gua", "ㄍㄨㄚ"},    {"gou", "ㄍㄡ"},      {"gen", "ㄍㄣ"},
    {"gei", "ㄍㄟ"},      {"gao", "ㄍㄠ"},      {"gan", "ㄍㄢ"},
    {"gai", "ㄍㄞ"},      {"fou", "ㄈㄡ"},      {"fen", "ㄈㄣ"},
    {"fei", "ㄈㄟ"},      {"fan", "ㄈㄢ"},      {"eng", "ㄥ"},
    {"duo", "ㄉㄨㄛ"},    {"dun", "ㄉㄨㄣ"},    {"dou", "ㄉㄡ"},
    {"dia", "ㄉㄧㄚ"},    {"dei", "ㄉㄟ"},      {"dao", "ㄉㄠ"},
    {"dan", "ㄉㄢ"},      {"dai", "ㄉㄞ"},      {"tsu", "ㄘㄨ"},
    {"chi", "ㄑㄧ"},      {"chu", "ㄔㄨ"},      {"che", "ㄔㄜ"},
    {"cha", "ㄔㄚ"},      {"tse", "ㄘㄜ"},      {"tsa", "ㄘㄚ"},
    {"bin", "ㄅㄧㄣ"},    {"ben", "ㄅㄣ"},      {"bei", "ㄅㄟ"},
    {"bao", "ㄅㄠ"},      {"ban", "ㄅㄢ"},      {"bai", "ㄅㄞ"},
    {"ang", "ㄤ"},        {"ch", "ㄑ"},         {"zu", "ㄗㄨ"},
    {"ze", "ㄗㄜ"},       {"za", "ㄗㄚ"},       {"yu", "ㄩ"},
    {"yo", "ㄧㄛ"},       {"ya", "ㄧㄚ"},       {"yi", "ㄧ"},
    {"wu", "ㄨ"},         {"wo", "ㄨㄛ"},       {"wa", "ㄨㄚ"},
    {"tu", "ㄊㄨ"},       {"ti", "ㄊㄧ"},       {"te", "ㄊㄜ"},
    {"ta", "ㄊㄚ"},       {"su", "ㄙㄨ"},       {"si", "ㄒㄧ"},
    {"se", "ㄙㄜ"},       {"sa", "ㄙㄚ"},       {"ru", "ㄖㄨ"},
    {"re", "ㄖㄜ"},       {"pu", "ㄆㄨ"},       {"po", "ㄆㄛ"},
    {"pi", "ㄆㄧ"},       {"pa", "ㄆㄚ"},       {"ou", "ㄡ"},
    {"nu", "ㄋㄨ"},       {"ni", "ㄋㄧ"},       {"ne", "ㄋㄜ"},
    {"na", "ㄋㄚ"},       {"mu", "ㄇㄨ"},       {"mo", "ㄇㄛ"},
    {"mi", "ㄇㄧ"},       {"me", "ㄇㄜ"},       {"ma", "ㄇㄚ"},
    {"lu", "ㄌㄨ"},       {"lo", "ㄌㄛ"},       {"li", "ㄌㄧ"},
    {"le", "ㄌㄜ"},       {"la", "ㄌㄚ"},       {"ku", "ㄎㄨ"},
    {"ke", "ㄎㄜ"},       {"ka", "ㄎㄚ"},       {"ji", "ㄐㄧ"},
    {"hu", "ㄏㄨ"},       {"he", "ㄏㄜ"},       {"ha", "ㄏㄚ"},
    {"gu", "ㄍㄨ"},       {"ge", "ㄍㄜ"},       {"ga", "ㄍㄚ"},
    {"fu", "ㄈㄨ"},       {"fo", "ㄈㄛ"},       {"fa", "ㄈㄚ"},
    {"er", "ㄦ"},         {"en", "ㄣ"},         {"ei", "ㄟ"},
    {"eh", "ㄝ"},         {"du", "ㄉㄨ"},       {"di", "ㄉㄧ"},
    {"de", "ㄉㄜ"},       {"da", "ㄉㄚ"},       {"bu", "ㄅㄨ"},
    {"bo", "ㄅㄛ"},       {"bi", "ㄅㄧ"},       {"ba", "ㄅㄚ"},
    {"ao", "ㄠ"},         {"an", "ㄢ"},         {"ai", "ㄞ"},
    {"o", "ㄛ"},          {"e", "ㄜ"},          {"a", "ㄚ"}};

/// 通用拼音排列專用處理陣列
inline static std::map<std::string, std::string> mapUniversalPinyin = {
    {"shuang", "ㄕㄨㄤ"}, {"jhuang", "ㄓㄨㄤ"}, {"chuang", "ㄔㄨㄤ"},
    {"biang", "ㄅㄧㄤ"},  {"duang", "ㄉㄨㄤ"},  {"cyuan", "ㄑㄩㄢ"},
    {"cyong", "ㄑㄩㄥ"},  {"ciang", "ㄑㄧㄤ"},  {"kyang", "ㄎㄧㄤ"},
    {"syuan", "ㄒㄩㄢ"},  {"syong", "ㄒㄩㄥ"},  {"sihei", "ㄙㄟ"},
    {"siang", "ㄒㄧㄤ"},  {"shuei", "ㄕㄨㄟ"},  {"shuan", "ㄕㄨㄢ"},
    {"shuai", "ㄕㄨㄞ"},  {"sheng", "ㄕㄥ"},    {"shang", "ㄕㄤ"},
    {"niang", "ㄋㄧㄤ"},  {"lyuan", "ㄌㄩㄢ"},  {"liang", "ㄌㄧㄤ"},
    {"kuang", "ㄎㄨㄤ"},  {"jyuan", "ㄐㄩㄢ"},  {"jyong", "ㄐㄩㄥ"},
    {"jiang", "ㄐㄧㄤ"},  {"jhuei", "ㄓㄨㄟ"},  {"jhuan", "ㄓㄨㄢ"},
    {"jhuai", "ㄓㄨㄞ"},  {"jhong", "ㄓㄨㄥ"},  {"jheng", "ㄓㄥ"},
    {"jhang", "ㄓㄤ"},    {"huang", "ㄏㄨㄤ"},  {"guang", "ㄍㄨㄤ"},
    {"chuei", "ㄔㄨㄟ"},  {"chuan", "ㄔㄨㄢ"},  {"chuai", "ㄔㄨㄞ"},
    {"chong", "ㄔㄨㄥ"},  {"cheng", "ㄔㄥ"},    {"chang", "ㄔㄤ"},
    {"cyue", "ㄑㄩㄝ"},   {"syue", "ㄒㄩㄝ"},   {"nyue", "ㄋㄩㄝ"},
    {"lyue", "ㄌㄩㄝ"},   {"jyue", "ㄐㄩㄝ"},   {"cyun", "ㄑㄩㄣ"},
    {"cuei", "ㄘㄨㄟ"},   {"cuan", "ㄘㄨㄢ"},   {"cong", "ㄘㄨㄥ"},
    {"ciou", "ㄑㄧㄡ"},   {"cing", "ㄑㄧㄥ"},   {"ciao", "ㄑㄧㄠ"},
    {"cian", "ㄑㄧㄢ"},   {"ceng", "ㄘㄥ"},     {"cang", "ㄘㄤ"},
    {"gyao", "ㄍㄧㄠ"},   {"fiao", "ㄈㄧㄠ"},   {"zuei", "ㄗㄨㄟ"},
    {"zuan", "ㄗㄨㄢ"},   {"zong", "ㄗㄨㄥ"},   {"zeng", "ㄗㄥ"},
    {"zang", "ㄗㄤ"},     {"yuan", "ㄩㄢ"},     {"yong", "ㄩㄥ"},
    {"ying", "ㄧㄥ"},     {"yang", "ㄧㄤ"},     {"wong", "ㄨㄥ"},
    {"wang", "ㄨㄤ"},     {"tuei", "ㄊㄨㄟ"},   {"tuan", "ㄊㄨㄢ"},
    {"tong", "ㄊㄨㄥ"},   {"ting", "ㄊㄧㄥ"},   {"tiao", "ㄊㄧㄠ"},
    {"tian", "ㄊㄧㄢ"},   {"teng", "ㄊㄥ"},     {"tang", "ㄊㄤ"},
    {"syun", "ㄒㄩㄣ"},   {"suei", "ㄙㄨㄟ"},   {"suan", "ㄙㄨㄢ"},
    {"song", "ㄙㄨㄥ"},   {"siou", "ㄒㄧㄡ"},   {"sing", "ㄒㄧㄥ"},
    {"siao", "ㄒㄧㄠ"},   {"sian", "ㄒㄧㄢ"},   {"shuo", "ㄕㄨㄛ"},
    {"shun", "ㄕㄨㄣ"},   {"shua", "ㄕㄨㄚ"},   {"shou", "ㄕㄡ"},
    {"shih", "ㄕ"},       {"shen", "ㄕㄣ"},     {"shei", "ㄕㄟ"},
    {"shao", "ㄕㄠ"},     {"shan", "ㄕㄢ"},     {"shai", "ㄕㄞ"},
    {"seng", "ㄙㄥ"},     {"sang", "ㄙㄤ"},     {"ruei", "ㄖㄨㄟ"},
    {"ruan", "ㄖㄨㄢ"},   {"rong", "ㄖㄨㄥ"},   {"reng", "ㄖㄥ"},
    {"rang", "ㄖㄤ"},     {"ping", "ㄆㄧㄥ"},   {"piao", "ㄆㄧㄠ"},
    {"pian", "ㄆㄧㄢ"},   {"peng", "ㄆㄥ"},     {"pang", "ㄆㄤ"},
    {"nuei", "ㄋㄨㄟ"},   {"nuan", "ㄋㄨㄢ"},   {"nong", "ㄋㄨㄥ"},
    {"niou", "ㄋㄧㄡ"},   {"ning", "ㄋㄧㄥ"},   {"niao", "ㄋㄧㄠ"},
    {"nian", "ㄋㄧㄢ"},   {"neng", "ㄋㄥ"},     {"nang", "ㄋㄤ"},
    {"miou", "ㄇㄧㄡ"},   {"ming", "ㄇㄧㄥ"},   {"miao", "ㄇㄧㄠ"},
    {"mian", "ㄇㄧㄢ"},   {"meng", "ㄇㄥ"},     {"mang", "ㄇㄤ"},
    {"luan", "ㄌㄨㄢ"},   {"long", "ㄌㄨㄥ"},   {"liou", "ㄌㄧㄡ"},
    {"ling", "ㄌㄧㄥ"},   {"liao", "ㄌㄧㄠ"},   {"lian", "ㄌㄧㄢ"},
    {"leng", "ㄌㄥ"},     {"lang", "ㄌㄤ"},     {"kuei", "ㄎㄨㄟ"},
    {"kuan", "ㄎㄨㄢ"},   {"kuai", "ㄎㄨㄞ"},   {"kong", "ㄎㄨㄥ"},
    {"keng", "ㄎㄥ"},     {"kang", "ㄎㄤ"},     {"jyun", "ㄐㄩㄣ"},
    {"jiou", "ㄐㄧㄡ"},   {"jing", "ㄐㄧㄥ"},   {"jiao", "ㄐㄧㄠ"},
    {"jian", "ㄐㄧㄢ"},   {"jhuo", "ㄓㄨㄛ"},   {"jhun", "ㄓㄨㄣ"},
    {"jhua", "ㄓㄨㄚ"},   {"jhou", "ㄓㄡ"},     {"jhih", "ㄓ"},
    {"jhen", "ㄓㄣ"},     {"jhei", "ㄓㄟ"},     {"jhao", "ㄓㄠ"},
    {"jhan", "ㄓㄢ"},     {"jhai", "ㄓㄞ"},     {"huei", "ㄏㄨㄟ"},
    {"huan", "ㄏㄨㄢ"},   {"huai", "ㄏㄨㄞ"},   {"hong", "ㄏㄨㄥ"},
    {"heng", "ㄏㄥ"},     {"hang", "ㄏㄤ"},     {"guei", "ㄍㄨㄟ"},
    {"guan", "ㄍㄨㄢ"},   {"guai", "ㄍㄨㄞ"},   {"gong", "ㄍㄨㄥ"},
    {"geng", "ㄍㄥ"},     {"gang", "ㄍㄤ"},     {"fong", "ㄈㄥ"},
    {"fang", "ㄈㄤ"},     {"duei", "ㄉㄨㄟ"},   {"duan", "ㄉㄨㄢ"},
    {"dong", "ㄉㄨㄥ"},   {"diou", "ㄉㄧㄡ"},   {"ding", "ㄉㄧㄥ"},
    {"diao", "ㄉㄧㄠ"},   {"dian", "ㄉㄧㄢ"},   {"deng", "ㄉㄥ"},
    {"dang", "ㄉㄤ"},     {"chuo", "ㄔㄨㄛ"},   {"chun", "ㄔㄨㄣ"},
    {"chua", "ㄔㄨㄚ"},   {"chou", "ㄔㄡ"},     {"chih", "ㄔ"},
    {"chen", "ㄔㄣ"},     {"chao", "ㄔㄠ"},     {"chan", "ㄔㄢ"},
    {"chai", "ㄔㄞ"},     {"bing", "ㄅㄧㄥ"},   {"biao", "ㄅㄧㄠ"},
    {"bian", "ㄅㄧㄢ"},   {"beng", "ㄅㄥ"},     {"bang", "ㄅㄤ"},
    {"cie", "ㄑㄧㄝ"},    {"yue", "ㄩㄝ"},      {"tie", "ㄊㄧㄝ"},
    {"sie", "ㄒㄧㄝ"},    {"pie", "ㄆㄧㄝ"},    {"nie", "ㄋㄧㄝ"},
    {"mie", "ㄇㄧㄝ"},    {"lie", "ㄌㄧㄝ"},    {"jie", "ㄐㄧㄝ"},
    {"die", "ㄉㄧㄝ"},    {"cyu", "ㄑㄩ"},      {"cuo", "ㄘㄨㄛ"},
    {"cun", "ㄘㄨㄣ"},    {"cou", "ㄘㄡ"},      {"cin", "ㄑㄧㄣ"},
    {"cih", "ㄘ"},        {"cia", "ㄑㄧㄚ"},    {"cen", "ㄘㄣ"},
    {"cao", "ㄘㄠ"},      {"can", "ㄘㄢ"},      {"cai", "ㄘㄞ"},
    {"bie", "ㄅㄧㄝ"},    {"gin", "ㄍㄧㄣ"},    {"den", "ㄉㄣ"},
    {"zuo", "ㄗㄨㄛ"},    {"zun", "ㄗㄨㄣ"},    {"zou", "ㄗㄡ"},
    {"zih", "ㄗ"},        {"zen", "ㄗㄣ"},      {"zei", "ㄗㄟ"},
    {"zao", "ㄗㄠ"},      {"zan", "ㄗㄢ"},      {"zai", "ㄗㄞ"},
    {"yun", "ㄩㄣ"},      {"you", "ㄧㄡ"},      {"yin", "ㄧㄣ"},
    {"yao", "ㄧㄠ"},      {"yan", "ㄧㄢ"},      {"yai", "ㄧㄞ"},
    {"wun", "ㄨㄣ"},      {"wei", "ㄨㄟ"},      {"wan", "ㄨㄢ"},
    {"wai", "ㄨㄞ"},      {"tuo", "ㄊㄨㄛ"},    {"tun", "ㄊㄨㄣ"},
    {"tou", "ㄊㄡ"},      {"tao", "ㄊㄠ"},      {"tan", "ㄊㄢ"},
    {"tai", "ㄊㄞ"},      {"syu", "ㄒㄩ"},      {"suo", "ㄙㄨㄛ"},
    {"sun", "ㄙㄨㄣ"},    {"sou", "ㄙㄡ"},      {"sin", "ㄒㄧㄣ"},
    {"sih", "ㄙ"},        {"sia", "ㄒㄧㄚ"},    {"shu", "ㄕㄨ"},
    {"she", "ㄕㄜ"},      {"sha", "ㄕㄚ"},      {"sen", "ㄙㄣ"},
    {"sao", "ㄙㄠ"},      {"san", "ㄙㄢ"},      {"sai", "ㄙㄞ"},
    {"ruo", "ㄖㄨㄛ"},    {"run", "ㄖㄨㄣ"},    {"rou", "ㄖㄡ"},
    {"rih", "ㄖ"},        {"ren", "ㄖㄣ"},      {"rao", "ㄖㄠ"},
    {"ran", "ㄖㄢ"},      {"pou", "ㄆㄡ"},      {"pin", "ㄆㄧㄣ"},
    {"pia", "ㄆㄧㄚ"},    {"pen", "ㄆㄣ"},      {"pei", "ㄆㄟ"},
    {"pao", "ㄆㄠ"},      {"pan", "ㄆㄢ"},      {"pai", "ㄆㄞ"},
    {"nyu", "ㄋㄩ"},      {"nuo", "ㄋㄨㄛ"},    {"nun", "ㄋㄨㄣ"},
    {"nou", "ㄋㄡ"},      {"nin", "ㄋㄧㄣ"},    {"nen", "ㄋㄣ"},
    {"nei", "ㄋㄟ"},      {"nao", "ㄋㄠ"},      {"nan", "ㄋㄢ"},
    {"nai", "ㄋㄞ"},      {"mou", "ㄇㄡ"},      {"min", "ㄇㄧㄣ"},
    {"men", "ㄇㄣ"},      {"mei", "ㄇㄟ"},      {"mao", "ㄇㄠ"},
    {"man", "ㄇㄢ"},      {"mai", "ㄇㄞ"},      {"lyu", "ㄌㄩ"},
    {"luo", "ㄌㄨㄛ"},    {"lun", "ㄌㄨㄣ"},    {"lou", "ㄌㄡ"},
    {"lin", "ㄌㄧㄣ"},    {"lia", "ㄌㄧㄚ"},    {"lei", "ㄌㄟ"},
    {"lao", "ㄌㄠ"},      {"lan", "ㄌㄢ"},      {"lai", "ㄌㄞ"},
    {"kuo", "ㄎㄨㄛ"},    {"kun", "ㄎㄨㄣ"},    {"kua", "ㄎㄨㄚ"},
    {"kou", "ㄎㄡ"},      {"ken", "ㄎㄣ"},      {"kao", "ㄎㄠ"},
    {"kan", "ㄎㄢ"},      {"kai", "ㄎㄞ"},      {"jyu", "ㄐㄩ"},
    {"jin", "ㄐㄧㄣ"},    {"jia", "ㄐㄧㄚ"},    {"jhu", "ㄓㄨ"},
    {"jhe", "ㄓㄜ"},      {"jha", "ㄓㄚ"},      {"huo", "ㄏㄨㄛ"},
    {"hun", "ㄏㄨㄣ"},    {"hua", "ㄏㄨㄚ"},    {"hou", "ㄏㄡ"},
    {"hen", "ㄏㄣ"},      {"hei", "ㄏㄟ"},      {"hao", "ㄏㄠ"},
    {"han", "ㄏㄢ"},      {"hai", "ㄏㄞ"},      {"guo", "ㄍㄨㄛ"},
    {"gun", "ㄍㄨㄣ"},    {"gue", "ㄍㄨㄜ"},    {"gua", "ㄍㄨㄚ"},
    {"gou", "ㄍㄡ"},      {"gen", "ㄍㄣ"},      {"gei", "ㄍㄟ"},
    {"gao", "ㄍㄠ"},      {"gan", "ㄍㄢ"},      {"gai", "ㄍㄞ"},
    {"fou", "ㄈㄡ"},      {"fen", "ㄈㄣ"},      {"fei", "ㄈㄟ"},
    {"fan", "ㄈㄢ"},      {"eng", "ㄥ"},        {"duo", "ㄉㄨㄛ"},
    {"dun", "ㄉㄨㄣ"},    {"dou", "ㄉㄡ"},      {"dia", "ㄉㄧㄚ"},
    {"dei", "ㄉㄟ"},      {"dao", "ㄉㄠ"},      {"dan", "ㄉㄢ"},
    {"dai", "ㄉㄞ"},      {"chu", "ㄔㄨ"},      {"che", "ㄔㄜ"},
    {"cha", "ㄔㄚ"},      {"bin", "ㄅㄧㄣ"},    {"ben", "ㄅㄣ"},
    {"bei", "ㄅㄟ"},      {"bao", "ㄅㄠ"},      {"ban", "ㄅㄢ"},
    {"bai", "ㄅㄞ"},      {"ang", "ㄤ"},        {"yia", "ㄧㄚ"},
    {"ye", "ㄧㄝ"},       {"cu", "ㄘㄨ"},       {"ci", "ㄑㄧ"},
    {"ce", "ㄘㄜ"},       {"ca", "ㄘㄚ"},       {"zu", "ㄗㄨ"},
    {"ze", "ㄗㄜ"},       {"za", "ㄗㄚ"},       {"yu", "ㄩ"},
    {"yo", "ㄧㄛ"},       {"yi", "ㄧ"},         {"wu", "ㄨ"},
    {"wo", "ㄨㄛ"},       {"wa", "ㄨㄚ"},       {"tu", "ㄊㄨ"},
    {"ti", "ㄊㄧ"},       {"te", "ㄊㄜ"},       {"ta", "ㄊㄚ"},
    {"su", "ㄙㄨ"},       {"si", "ㄒㄧ"},       {"se", "ㄙㄜ"},
    {"sa", "ㄙㄚ"},       {"ru", "ㄖㄨ"},       {"re", "ㄖㄜ"},
    {"pu", "ㄆㄨ"},       {"po", "ㄆㄛ"},       {"pi", "ㄆㄧ"},
    {"pa", "ㄆㄚ"},       {"ou", "ㄡ"},         {"nu", "ㄋㄨ"},
    {"ni", "ㄋㄧ"},       {"ne", "ㄋㄜ"},       {"na", "ㄋㄚ"},
    {"mu", "ㄇㄨ"},       {"mo", "ㄇㄛ"},       {"mi", "ㄇㄧ"},
    {"me", "ㄇㄜ"},       {"ma", "ㄇㄚ"},       {"lu", "ㄌㄨ"},
    {"lo", "ㄌㄛ"},       {"li", "ㄌㄧ"},       {"le", "ㄌㄜ"},
    {"la", "ㄌㄚ"},       {"ku", "ㄎㄨ"},       {"ke", "ㄎㄜ"},
    {"ka", "ㄎㄚ"},       {"ji", "ㄐㄧ"},       {"hu", "ㄏㄨ"},
    {"he", "ㄏㄜ"},       {"ha", "ㄏㄚ"},       {"gu", "ㄍㄨ"},
    {"ge", "ㄍㄜ"},       {"ga", "ㄍㄚ"},       {"fu", "ㄈㄨ"},
    {"fo", "ㄈㄛ"},       {"fa", "ㄈㄚ"},       {"er", "ㄦ"},
    {"en", "ㄣ"},         {"ei", "ㄟ"},         {"eh", "ㄝ"},
    {"du", "ㄉㄨ"},       {"di", "ㄉㄧ"},       {"de", "ㄉㄜ"},
    {"da", "ㄉㄚ"},       {"bu", "ㄅㄨ"},       {"bo", "ㄅㄛ"},
    {"bi", "ㄅㄧ"},       {"ba", "ㄅㄚ"},       {"ao", "ㄠ"},
    {"an", "ㄢ"},         {"ai", "ㄞ"},         {"c", "ㄑ"},
    {"o", "ㄛ"},          {"e", "ㄜ"},          {"a", "ㄚ"}};

/// 韋氏拼音排列專用處理陣列
inline static std::map<std::string, std::string> mapWadeGilesPinyin = {
    {"a", "ㄚ"},           {"ai", "ㄞ"},         {"an", "ㄢ"},
    {"ang", "ㄤ"},         {"ao", "ㄠ"},         {"cha", "ㄓㄚ"},
    {"chai", "ㄓㄞ"},      {"chan", "ㄓㄢ"},     {"chang", "ㄓㄤ"},
    {"chao", "ㄓㄠ"},      {"che", "ㄓㄜ"},      {"chei", "ㄓㄟ"},
    {"chen", "ㄓㄣ"},      {"cheng", "ㄓㄥ"},    {"chi", "ㄐㄧ"},
    {"chia", "ㄐㄧㄚ"},    {"chiang", "ㄐㄧㄤ"}, {"chiao", "ㄐㄧㄠ"},
    {"chieh", "ㄐㄧㄝ"},   {"chien", "ㄐㄧㄢ"},  {"chih", "ㄓ"},
    {"chin", "ㄐㄧㄣ"},    {"ching", "ㄐㄧㄥ"},  {"chiu", "ㄐㄧㄡ"},
    {"chiung", "ㄐㄩㄥ"},  {"cho", "ㄓㄨㄛ"},    {"chou", "ㄓㄡ"},
    {"chu", "ㄓㄨ"},       {"chua", "ㄓㄨㄚ"},   {"chuai", "ㄓㄨㄞ"},
    {"chuan", "ㄓㄨㄢ"},   {"chuang", "ㄓㄨㄤ"}, {"chui", "ㄓㄨㄟ"},
    {"chun", "ㄓㄨㄣ"},    {"chung", "ㄓㄨㄥ"},  {"ch'a", "ㄔㄚ"},
    {"ch'ai", "ㄔㄞ"},     {"ch'an", "ㄔㄢ"},    {"ch'ang", "ㄔㄤ"},
    {"ch'ao", "ㄔㄠ"},     {"ch'e", "ㄔㄜ"},     {"ch'en", "ㄔㄣ"},
    {"ch'eng", "ㄔㄥ"},    {"ch'i", "ㄑㄧ"},     {"ch'ia", "ㄑㄧㄚ"},
    {"ch'iang", "ㄑㄧㄤ"}, {"ch'iao", "ㄑㄧㄠ"}, {"ch'ieh", "ㄑㄧㄝ"},
    {"ch'ien", "ㄑㄧㄢ"},  {"ch'ih", "ㄔ"},      {"ch'in", "ㄑㄧㄣ"},
    {"ch'ing", "ㄑㄧㄥ"},  {"ch'iu", "ㄑㄧㄡ"},  {"ch'iung", "ㄑㄩㄥ"},
    {"ch'o", "ㄔㄨㄛ"},    {"ch'ou", "ㄔㄡ"},    {"ch'u", "ㄔㄨ"},
    {"ch'ua", "ㄔㄨㄚ"},   {"ch'uai", "ㄔㄨㄞ"}, {"ch'uan", "ㄔㄨㄢ"},
    {"ch'uang", "ㄔㄨㄤ"}, {"ch'ui", "ㄔㄨㄟ"},  {"ch'un", "ㄔㄨㄣ"},
    {"ch'ung", "ㄔㄨㄥ"},  {"ch'v", "ㄑㄩ"},     {"ch'van", "ㄑㄩㄢ"},
    {"ch'veh", "ㄑㄩㄝ"},  {"ch'vn", "ㄑㄩㄣ"},  {"chv", "ㄐㄩ"},
    {"chvan", "ㄐㄩㄢ"},   {"chveh", "ㄐㄩㄝ"},  {"chvn", "ㄐㄩㄣ"},
    {"e", "ㄜ"},           {"ei", "ㄟ"},         {"en", "ㄣ"},
    {"erh", "ㄦ"},         {"fa", "ㄈㄚ"},       {"fan", "ㄈㄢ"},
    {"fang", "ㄈㄤ"},      {"fei", "ㄈㄟ"},      {"fen", "ㄈㄣ"},
    {"feng", "ㄈㄥ"},      {"fo", "ㄈㄛ"},       {"fou", "ㄈㄡ"},
    {"fu", "ㄈㄨ"},        {"ha", "ㄏㄚ"},       {"hai", "ㄏㄞ"},
    {"han", "ㄏㄢ"},       {"hang", "ㄏㄤ"},     {"hao", "ㄏㄠ"},
    {"hei", "ㄏㄟ"},       {"hen", "ㄏㄣ"},      {"heng", "ㄏㄥ"},
    {"ho", "ㄏㄜ"},        {"hou", "ㄏㄡ"},      {"hsi", "ㄒㄧ"},
    {"hsia", "ㄒㄧㄚ"},    {"hsiang", "ㄒㄧㄤ"}, {"hsiao", "ㄒㄧㄠ"},
    {"hsieh", "ㄒㄧㄝ"},   {"hsien", "ㄒㄧㄢ"},  {"hsin", "ㄒㄧㄣ"},
    {"hsing", "ㄒㄧㄥ"},   {"hsiu", "ㄒㄧㄡ"},   {"hsiung", "ㄒㄩㄥ"},
    {"hsv", "ㄒㄩ"},       {"hsvan", "ㄒㄩㄢ"},  {"hsveh", "ㄒㄩㄝ"},
    {"hsvn", "ㄒㄩㄣ"},    {"hu", "ㄏㄨ"},       {"hua", "ㄏㄨㄚ"},
    {"huai", "ㄏㄨㄞ"},    {"huan", "ㄏㄨㄢ"},   {"huang", "ㄏㄨㄤ"},
    {"hui", "ㄏㄨㄟ"},     {"hun", "ㄏㄨㄣ"},    {"hung", "ㄏㄨㄥ"},
    {"huo", "ㄏㄨㄛ"},     {"i", "ㄧ"},          {"jan", "ㄖㄢ"},
    {"jang", "ㄖㄤ"},      {"jao", "ㄖㄠ"},      {"je", "ㄖㄜ"},
    {"jen", "ㄖㄣ"},       {"jeng", "ㄖㄥ"},     {"jih", "ㄖ"},
    {"jo", "ㄖㄨㄛ"},      {"jou", "ㄖㄡ"},      {"ju", "ㄖㄨ"},
    {"juan", "ㄖㄨㄢ"},    {"jui", "ㄖㄨㄟ"},    {"jun", "ㄖㄨㄣ"},
    {"jung", "ㄖㄨㄥ"},    {"ka", "ㄍㄚ"},       {"kai", "ㄍㄞ"},
    {"kan", "ㄍㄢ"},       {"kang", "ㄍㄤ"},     {"kao", "ㄍㄠ"},
    {"kei", "ㄍㄟ"},       {"ken", "ㄍㄣ"},      {"keng", "ㄍㄥ"},
    {"ko", "ㄍㄜ"},        {"kou", "ㄍㄡ"},      {"ku", "ㄍㄨ"},
    {"kua", "ㄍㄨㄚ"},     {"kuai", "ㄍㄨㄞ"},   {"kuan", "ㄍㄨㄢ"},
    {"kuang", "ㄍㄨㄤ"},   {"kuei", "ㄍㄨㄟ"},   {"kun", "ㄍㄨㄣ"},
    {"kung", "ㄍㄨㄥ"},    {"kuo", "ㄍㄨㄛ"},    {"k'a", "ㄎㄚ"},
    {"k'ai", "ㄎㄞ"},      {"k'an", "ㄎㄢ"},     {"k'ang", "ㄎㄤ"},
    {"k'ao", "ㄎㄠ"},      {"k'en", "ㄎㄣ"},     {"k'eng", "ㄎㄥ"},
    {"k'o", "ㄎㄜ"},       {"k'ou", "ㄎㄡ"},     {"k'u", "ㄎㄨ"},
    {"k'ua", "ㄎㄨㄚ"},    {"k'uai", "ㄎㄨㄞ"},  {"k'uan", "ㄎㄨㄢ"},
    {"k'uang", "ㄎㄨㄤ"},  {"k'uei", "ㄎㄨㄟ"},  {"k'un", "ㄎㄨㄣ"},
    {"k'ung", "ㄎㄨㄥ"},   {"k'uo", "ㄎㄨㄛ"},   {"la", "ㄌㄚ"},
    {"lai", "ㄌㄞ"},       {"lan", "ㄌㄢ"},      {"lang", "ㄌㄤ"},
    {"lao", "ㄌㄠ"},       {"le", "ㄌㄜ"},       {"lei", "ㄌㄟ"},
    {"leng", "ㄌㄥ"},      {"li", "ㄌㄧ"},       {"lia", "ㄌㄧㄚ"},
    {"liang", "ㄌㄧㄤ"},   {"liao", "ㄌㄧㄠ"},   {"lieh", "ㄌㄧㄝ"},
    {"lien", "ㄌㄧㄢ"},    {"lin", "ㄌㄧㄣ"},    {"ling", "ㄌㄧㄥ"},
    {"liu", "ㄌㄧㄡ"},     {"lo", "ㄌㄨㄛ"},     {"lou", "ㄌㄡ"},
    {"lu", "ㄌㄨ"},        {"luan", "ㄌㄨㄢ"},   {"lun", "ㄌㄨㄣ"},
    {"lung", "ㄌㄨㄥ"},    {"lv", "ㄌㄩ"},       {"lveh", "ㄌㄩㄝ"},
    {"lvn", "ㄌㄩㄣ"},     {"ma", "ㄇㄚ"},       {"mai", "ㄇㄞ"},
    {"man", "ㄇㄢ"},       {"mang", "ㄇㄤ"},     {"mao", "ㄇㄠ"},
    {"me", "ㄇㄜ"},        {"mei", "ㄇㄟ"},      {"men", "ㄇㄣ"},
    {"meng", "ㄇㄥ"},      {"mi", "ㄇㄧ"},       {"miao", "ㄇㄧㄠ"},
    {"mieh", "ㄇㄧㄝ"},    {"mien", "ㄇㄧㄢ"},   {"min", "ㄇㄧㄣ"},
    {"ming", "ㄇㄧㄥ"},    {"miu", "ㄇㄧㄡ"},    {"mo", "ㄇㄛ"},
    {"mou", "ㄇㄡ"},       {"mu", "ㄇㄨ"},       {"na", "ㄋㄚ"},
    {"nai", "ㄋㄞ"},       {"nan", "ㄋㄢ"},      {"nang", "ㄋㄤ"},
    {"nao", "ㄋㄠ"},       {"ne", "ㄋㄜ"},       {"nei", "ㄋㄟ"},
    {"nen", "ㄋㄣ"},       {"neng", "ㄋㄥ"},     {"ni", "ㄋㄧ"},
    {"nia", "ㄋㄧㄚ"},     {"niang", "ㄋㄧㄤ"},  {"niao", "ㄋㄧㄠ"},
    {"nieh", "ㄋㄧㄝ"},    {"nien", "ㄋㄧㄢ"},   {"nin", "ㄋㄧㄣ"},
    {"ning", "ㄋㄧㄥ"},    {"niu", "ㄋㄧㄡ"},    {"no", "ㄋㄨㄛ"},
    {"nou", "ㄋㄡ"},       {"nu", "ㄋㄨ"},       {"nuan", "ㄋㄨㄢ"},
    {"nun", "ㄋㄨㄣ"},     {"nung", "ㄋㄨㄥ"},   {"nv", "ㄋㄩ"},
    {"nveh", "ㄋㄩㄝ"},    {"ou", "ㄡ"},         {"pa", "ㄅㄚ"},
    {"pai", "ㄅㄞ"},       {"pan", "ㄅㄢ"},      {"pang", "ㄅㄤ"},
    {"pao", "ㄅㄠ"},       {"pei", "ㄅㄟ"},      {"pen", "ㄅㄣ"},
    {"peng", "ㄅㄥ"},      {"pi", "ㄅㄧ"},       {"piao", "ㄅㄧㄠ"},
    {"pieh", "ㄅㄧㄝ"},    {"pien", "ㄅㄧㄢ"},   {"pin", "ㄅㄧㄣ"},
    {"ping", "ㄅㄧㄥ"},    {"po", "ㄅㄛ"},       {"pu", "ㄅㄨ"},
    {"p'a", "ㄆㄚ"},       {"p'ai", "ㄆㄞ"},     {"p'an", "ㄆㄢ"},
    {"p'ang", "ㄆㄤ"},     {"p'ao", "ㄆㄠ"},     {"p'ei", "ㄆㄟ"},
    {"p'en", "ㄆㄣ"},      {"p'eng", "ㄆㄥ"},    {"p'i", "ㄆㄧ"},
    {"p'iao", "ㄆㄧㄠ"},   {"p'ieh", "ㄆㄧㄝ"},  {"p'ien", "ㄆㄧㄢ"},
    {"p'in", "ㄆㄧㄣ"},    {"p'ing", "ㄆㄧㄥ"},  {"p'o", "ㄆㄛ"},
    {"p'ou", "ㄆㄡ"},      {"p'u", "ㄆㄨ"},      {"sa", "ㄙㄚ"},
    {"sai", "ㄙㄞ"},       {"san", "ㄙㄢ"},      {"sang", "ㄙㄤ"},
    {"sao", "ㄙㄠ"},       {"se", "ㄙㄜ"},       {"sei", "ㄙㄟ"},
    {"sen", "ㄙㄣ"},       {"seng", "ㄙㄥ"},     {"sha", "ㄕㄚ"},
    {"shai", "ㄕㄞ"},      {"shan", "ㄕㄢ"},     {"shang", "ㄕㄤ"},
    {"shao", "ㄕㄠ"},      {"she", "ㄕㄜ"},      {"shei", "ㄕㄟ"},
    {"shen", "ㄕㄣ"},      {"sheng", "ㄕㄥ"},    {"shih", "ㄕ"},
    {"shou", "ㄕㄡ"},      {"shu", "ㄕㄨ"},      {"shua", "ㄕㄨㄚ"},
    {"shuai", "ㄕㄨㄞ"},   {"shuan", "ㄕㄨㄢ"},  {"shuang", "ㄕㄨㄤ"},
    {"shui", "ㄕㄨㄟ"},    {"shun", "ㄕㄨㄣ"},   {"shung", "ㄕㄨㄥ"},
    {"shuo", "ㄕㄨㄛ"},    {"so", "ㄙㄨㄛ"},     {"sou", "ㄙㄡ"},
    {"ssu", "ㄙ"},         {"su", "ㄙㄨ"},       {"suan", "ㄙㄨㄢ"},
    {"sui", "ㄙㄨㄟ"},     {"sun", "ㄙㄨㄣ"},    {"sung", "ㄙㄨㄥ"},
    {"ta", "ㄉㄚ"},        {"tai", "ㄉㄞ"},      {"tan", "ㄉㄢ"},
    {"tang", "ㄉㄤ"},      {"tao", "ㄉㄠ"},      {"te", "ㄉㄜ"},
    {"tei", "ㄉㄟ"},       {"ten", "ㄉㄣ"},      {"teng", "ㄉㄥ"},
    {"ti", "ㄉㄧ"},        {"tiang", "ㄉㄧㄤ"},  {"tiao", "ㄉㄧㄠ"},
    {"tieh", "ㄉㄧㄝ"},    {"tien", "ㄉㄧㄢ"},   {"ting", "ㄉㄧㄥ"},
    {"tiu", "ㄉㄧㄡ"},     {"to", "ㄉㄨㄛ"},     {"tou", "ㄉㄡ"},
    {"tsa", "ㄗㄚ"},       {"tsai", "ㄗㄞ"},     {"tsan", "ㄗㄢ"},
    {"tsang", "ㄗㄤ"},     {"tsao", "ㄗㄠ"},     {"tse", "ㄗㄜ"},
    {"tsei", "ㄗㄟ"},      {"tsen", "ㄗㄣ"},     {"tseng", "ㄗㄥ"},
    {"tso", "ㄗㄨㄛ"},     {"tsou", "ㄗㄡ"},     {"tsu", "ㄗㄨ"},
    {"tsuan", "ㄗㄨㄢ"},   {"tsui", "ㄗㄨㄟ"},   {"tsun", "ㄗㄨㄣ"},
    {"tsung", "ㄗㄨㄥ"},   {"ts'a", "ㄘㄚ"},     {"ts'ai", "ㄘㄞ"},
    {"ts'an", "ㄘㄢ"},     {"ts'ang", "ㄘㄤ"},   {"ts'ao", "ㄘㄠ"},
    {"ts'e", "ㄘㄜ"},      {"ts'en", "ㄘㄣ"},    {"ts'eng", "ㄘㄥ"},
    {"ts'o", "ㄘㄨㄛ"},    {"ts'ou", "ㄘㄡ"},    {"ts'u", "ㄘㄨ"},
    {"ts'uan", "ㄘㄨㄢ"},  {"ts'ui", "ㄘㄨㄟ"},  {"ts'un", "ㄘㄨㄣ"},
    {"ts'ung", "ㄘㄨㄥ"},  {"tu", "ㄉㄨ"},       {"tuan", "ㄉㄨㄢ"},
    {"tui", "ㄉㄨㄟ"},     {"tun", "ㄉㄨㄣ"},    {"tung", "ㄉㄨㄥ"},
    {"tzu", "ㄗ"},         {"tz'u", "ㄘ"},       {"t'a", "ㄊㄚ"},
    {"t'ai", "ㄊㄞ"},      {"t'an", "ㄊㄢ"},     {"t'ang", "ㄊㄤ"},
    {"t'ao", "ㄊㄠ"},      {"t'e", "ㄊㄜ"},      {"t'eng", "ㄊㄥ"},
    {"t'i", "ㄊㄧ"},       {"t'iao", "ㄊㄧㄠ"},  {"t'ieh", "ㄊㄧㄝ"},
    {"t'ien", "ㄊㄧㄢ"},   {"t'ing", "ㄊㄧㄥ"},  {"t'o", "ㄊㄨㄛ"},
    {"t'ou", "ㄊㄡ"},      {"t'u", "ㄊㄨ"},      {"t'uan", "ㄊㄨㄢ"},
    {"t'ui", "ㄊㄨㄟ"},    {"t'un", "ㄊㄨㄣ"},   {"t'ung", "ㄊㄨㄥ"},
    {"wa", "ㄨㄚ"},        {"wai", "ㄨㄞ"},      {"wan", "ㄨㄢ"},
    {"wang", "ㄨㄤ"},      {"wei", "ㄨㄟ"},      {"wen", "ㄨㄣ"},
    {"weng", "ㄨㄥ"},      {"wo", "ㄨㄛ"},       {"wu", "ㄨ"},
    {"ya", "ㄧㄚ"},        {"yan", "ㄧㄢ"},      {"yang", "ㄧㄤ"},
    {"yao", "ㄧㄠ"},       {"yeh", "ㄧㄝ"},      {"yin", "ㄧㄣ"},
    {"ying", "ㄧㄥ"},      {"yu", "ㄧㄡ"},       {"yung", "ㄩㄥ"},
    {"yv", "ㄩ"},          {"yvan", "ㄩㄢ"},     {"yveh", "ㄩㄝ"},
    {"yvn", "ㄩㄣ"}};

// MARK: - Maps for Keyboard-to-Phonabet parsers

/// 標準大千排列專用處理陣列。
///
/// 威注音輸入法 macOS 版使用了 Ukelele 佈局來完成對
/// 諸如倚天傳統等其它注音鍵盤排列的支援。如果要將鐵
/// 恨模組拿給別的平台的輸入法使用的話，恐怕需要針對
/// 這些注音鍵盤排列各自新增專用陣列才可以。
inline static std::map<std::string, std::string> mapQwertyDachen = {
    {"0", "ㄢ"}, {"1", "ㄅ"}, {"2", "ㄉ"}, {"3", "ˇ"},  {"4", "ˋ"},
    {"5", "ㄓ"}, {"6", "ˊ"},  {"7", "˙"},  {"8", "ㄚ"}, {"9", "ㄞ"},
    {"-", "ㄦ"}, {",", "ㄝ"}, {".", "ㄡ"}, {"/", "ㄥ"}, {";", "ㄤ"},
    {"a", "ㄇ"}, {"b", "ㄖ"}, {"c", "ㄏ"}, {"d", "ㄎ"}, {"e", "ㄍ"},
    {"f", "ㄑ"}, {"g", "ㄕ"}, {"h", "ㄘ"}, {"i", "ㄛ"}, {"j", "ㄨ"},
    {"k", "ㄜ"}, {"l", "ㄠ"}, {"m", "ㄩ"}, {"n", "ㄙ"}, {"o", "ㄟ"},
    {"p", "ㄣ"}, {"q", "ㄆ"}, {"r", "ㄐ"}, {"s", "ㄋ"}, {"t", "ㄔ"},
    {"u", "ㄧ"}, {"v", "ㄒ"}, {"w", "ㄊ"}, {"x", "ㄌ"}, {"y", "ㄗ"},
    {"z", "ㄈ"}, {" ", " "}};

/// 酷音大千二十六鍵排列專用處理陣列，但未包含全部的處理內容。
///
/// 在這裡將二十六個字母寫全，也只是為了方便做 validity check。
/// 這裡提前對複音按鍵做處理，然後再用程式判斷介母類型、
/// 據此判斷是否需要做複音切換。
inline static std::map<std::string, std::string> mapDachenCP26StaticKeys = {
    {"a", "ㄇ"}, {"b", "ㄖ"}, {"c", "ㄏ"}, {"d", "ㄎ"}, {"e", "ㄍ"},
    {"f", "ㄑ"}, {"g", "ㄕ"}, {"h", "ㄘ"}, {"i", "ㄞ"}, {"j", "ㄨ"},
    {"k", "ㄜ"}, {"l", "ㄤ"}, {"m", "ㄩ"}, {"n", "ㄙ"}, {"o", "ㄢ"},
    {"p", "ㄦ"}, {"q", "ㄅ"}, {"r", "ㄐ"}, {"s", "ㄋ"}, {"t", "ㄓ"},
    {"u", "ㄧ"}, {"v", "ㄒ"}, {"w", "ㄉ"}, {"x", "ㄌ"}, {"y", "ㄗ"},
    {"z", "ㄈ"}, {" ", " "}};

/// 許氏排列專用處理陣列，但未包含全部的映射內容。
///
/// 在這裡將二十六個字母寫全，也只是為了方便做 validity check。
/// 這裡提前對複音按鍵做處理，然後再用程式判斷介母類型、
/// 據此判斷是否需要做複音切換。
inline static std::map<std::string, std::string> mapHsuStaticKeys = {
    {"a", "ㄘ"}, {"b", "ㄅ"}, {"c", "ㄕ"}, {"d", "ㄉ"}, {"e", "ㄧ"},
    {"f", "ㄈ"}, {"g", "ㄍ"}, {"h", "ㄏ"}, {"i", "ㄞ"}, {"j", "ㄐ"},
    {"k", "ㄎ"}, {"l", "ㄌ"}, {"m", "ㄇ"}, {"n", "ㄋ"}, {"o", "ㄡ"},
    {"p", "ㄆ"}, {"r", "ㄖ"}, {"s", "ㄙ"}, {"t", "ㄊ"}, {"u", "ㄩ"},
    {"v", "ㄔ"}, {"w", "ㄠ"}, {"x", "ㄨ"}, {"y", "ㄚ"}, {"z", "ㄗ"},
    {" ", " "}};

/// 星光排列專用處理陣列，但未包含全部的映射內容。
///
/// 在這裡將二十六個字母寫全，也只是為了方便做 validity check。
/// 這裡提前對複音按鍵做處理，然後再用程式判斷介母類型、
/// 據此判斷是否需要做複音切換。
inline static std::map<std::string, std::string> mapStarlightStaticKeys = {
    {"a", "ㄚ"}, {"b", "ㄅ"}, {"c", "ㄘ"}, {"d", "ㄉ"}, {"e", "ㄜ"},
    {"f", "ㄈ"}, {"g", "ㄍ"}, {"h", "ㄏ"}, {"i", "ㄧ"}, {"j", "ㄓ"},
    {"k", "ㄎ"}, {"l", "ㄌ"}, {"m", "ㄇ"}, {"n", "ㄋ"}, {"o", "ㄛ"},
    {"p", "ㄆ"}, {"q", "ㄔ"}, {"r", "ㄖ"}, {"s", "ㄙ"}, {"t", "ㄊ"},
    {"u", "ㄨ"}, {"v", "ㄩ"}, {"w", "ㄡ"}, {"x", "ㄕ"}, {"y", "ㄞ"},
    {"z", "ㄗ"}, {" ", " "},  {"1", " "},  {"2", "ˊ"},  {"3", "ˇ"},
    {"4", "ˋ"},  {"5", "˙"},  {"6", " "},  {"7", "ˊ"},  {"8", "ˇ"},
    {"9", "ˋ"},  {"0", "˙"}};

/// 倚天忘形排列預處理專用陣列，但未包含全部的映射內容。
///
/// 在這裡將二十六個字母寫全，也只是為了方便做 validity check。
/// 這裡提前對複音按鍵做處理，然後再用程式判斷介母類型、
/// 據此判斷是否需要做複音切換。
inline static std::map<std::string, std::string> mapETen26StaticKeys = {
    {"a", "ㄚ"}, {"b", "ㄅ"}, {"c", "ㄕ"}, {"d", "ㄉ"}, {"e", "ㄧ"},
    {"f", "ㄈ"}, {"g", "ㄓ"}, {"h", "ㄏ"}, {"i", "ㄞ"}, {"j", "ㄖ"},
    {"k", "ㄎ"}, {"l", "ㄌ"}, {"m", "ㄇ"}, {"n", "ㄋ"}, {"o", "ㄛ"},
    {"p", "ㄆ"}, {"q", "ㄗ"}, {"r", "ㄜ"}, {"s", "ㄙ"}, {"t", "ㄊ"},
    {"u", "ㄩ"}, {"v", "ㄍ"}, {"w", "ㄘ"}, {"x", "ㄨ"}, {"y", "ㄔ"},
    {"z", "ㄠ"}, {" ", " "}};

/// 劉氏擬音注音排列預處理專用陣列，但未包含全部的映射內容。
///
/// 在這裡將二十六個字母寫全，也只是為了方便做 validity check。
/// 這裡提前對複音按鍵做處理，然後再用程式判斷介母類型、
/// 據此判斷是否需要做複音切換。
inline static std::map<std::string, std::string> mapAlvinLiuStaticKeys = {
    {"q", "ㄑ"}, {"w", "ㄠ"}, {"e", "ㄜ"}, {"r", "ㄖ"}, {"t", "ㄊ"},
    {"y", "ㄩ"}, {"u", "ㄨ"}, {"i", "ㄧ"}, {"o", "ㄛ"}, {"p", "ㄆ"},
    {"a", "ㄚ"}, {"s", "ㄙ"}, {"d", "ㄉ"}, {"f", "ㄈ"}, {"g", "ㄍ"},
    {"h", "ㄏ"}, {"j", "ㄐ"}, {"k", "ㄎ"}, {"l", "ㄦ"}, {"z", "ㄗ"},
    {"x", "ㄒ"}, {"c", "ㄘ"}, {"v", "ㄡ"}, {"b", "ㄅ"}, {"n", "ㄋ"},
    {"m", "ㄇ"}, {" ", " "}};

/// 倚天傳統排列專用處理陣列。
inline static std::map<std::string, std::string> mapQwertyETenTraditional = {
    {"'", "ㄘ"}, {",", "ㄓ"}, {"-", "ㄥ"}, {".", "ㄔ"}, {"/", "ㄕ"},
    {"0", "ㄤ"}, {"1", "˙"},  {"2", "ˊ"},  {"3", "ˇ"},  {"4", "ˋ"},
    {"7", "ㄑ"}, {"8", "ㄢ"}, {"9", "ㄣ"}, {";", "ㄗ"}, {"=", "ㄦ"},
    {"a", "ㄚ"}, {"b", "ㄅ"}, {"c", "ㄒ"}, {"d", "ㄉ"}, {"e", "ㄧ"},
    {"f", "ㄈ"}, {"g", "ㄐ"}, {"h", "ㄏ"}, {"i", "ㄞ"}, {"j", "ㄖ"},
    {"k", "ㄎ"}, {"l", "ㄌ"}, {"m", "ㄇ"}, {"n", "ㄋ"}, {"o", "ㄛ"},
    {"p", "ㄆ"}, {"q", "ㄟ"}, {"r", "ㄜ"}, {"s", "ㄙ"}, {"t", "ㄊ"},
    {"u", "ㄩ"}, {"v", "ㄍ"}, {"w", "ㄝ"}, {"x", "ㄨ"}, {"y", "ㄡ"},
    {"z", "ㄠ"}, {" ", " "}};

/// IBM排列專用處理陣列。
inline static std::map<std::string, std::string> mapQwertyIBM = {
    {",", "ˇ"},  {"-", "ㄏ"}, {".", "ˋ"},  {"/", "˙"},  {"0", "ㄎ"},
    {"1", "ㄅ"}, {"2", "ㄆ"}, {"3", "ㄇ"}, {"4", "ㄈ"}, {"5", "ㄉ"},
    {"6", "ㄊ"}, {"7", "ㄋ"}, {"8", "ㄌ"}, {"9", "ㄍ"}, {";", "ㄠ"},
    {"a", "ㄧ"}, {"b", "ㄥ"}, {"c", "ㄣ"}, {"d", "ㄩ"}, {"e", "ㄒ"},
    {"f", "ㄚ"}, {"g", "ㄛ"}, {"h", "ㄜ"}, {"i", "ㄗ"}, {"j", "ㄝ"},
    {"k", "ㄞ"}, {"l", "ㄟ"}, {"m", "ˊ"},  {"n", "ㄦ"}, {"o", "ㄘ"},
    {"p", "ㄙ"}, {"q", "ㄐ"}, {"r", "ㄓ"}, {"s", "ㄨ"}, {"t", "ㄔ"},
    {"u", "ㄖ"}, {"v", "ㄤ"}, {"w", "ㄑ"}, {"x", "ㄢ"}, {"y", "ㄕ"},
    {"z", "ㄡ"}, {" ", " "}};

/// 精業排列專用處理陣列。
inline static std::map<std::string, std::string> mapSeigyou = {
    {"a", "ˇ"},  {"b", "ㄒ"}, {"c", "ㄌ"}, {"d", "ㄋ"}, {"e", "ㄊ"},
    {"f", "ㄎ"}, {"g", "ㄑ"}, {"h", "ㄕ"}, {"i", "ㄛ"}, {"j", "ㄘ"},
    {"k", "ㄜ"}, {"l", "ㄠ"}, {"m", "ㄙ"}, {"n", "ㄖ"}, {"o", "ㄟ"},
    {"p", "ㄣ"}, {"q", "ˊ"},  {"r", "ㄍ"}, {"s", "ㄇ"}, {"t", "ㄐ"},
    {"u", "ㄗ"}, {"v", "ㄏ"}, {"w", "ㄆ"}, {"x", "ㄈ"}, {"y", "ㄔ"},
    {"z", "ˋ"},  {"1", "˙"},  {"2", "ㄅ"}, {"3", "ㄉ"}, {"6", "ㄓ"},
    {"8", "ㄚ"}, {"9", "ㄞ"}, {"0", "ㄢ"}, {"-", "ㄧ"}, {";", "ㄤ"},
    {",", "ㄝ"}, {".", "ㄡ"}, {"/", "ㄥ"}, {"'", "ㄩ"}, {"{", "ㄨ"},
    {"=", "ㄦ"}, {" ", " "}};

/// 偽精業排列專用處理陣列。
inline static std::map<std::string, std::string> mapFakeSeigyou = {
    {"a", "ˇ"},  {"b", "ㄒ"}, {"c", "ㄌ"}, {"d", "ㄋ"}, {"e", "ㄊ"},
    {"f", "ㄎ"}, {"g", "ㄑ"}, {"h", "ㄕ"}, {"i", "ㄛ"}, {"j", "ㄘ"},
    {"k", "ㄜ"}, {"l", "ㄠ"}, {"m", "ㄙ"}, {"n", "ㄖ"}, {"o", "ㄟ"},
    {"p", "ㄣ"}, {"q", "ˊ"},  {"r", "ㄍ"}, {"s", "ㄇ"}, {"t", "ㄐ"},
    {"u", "ㄗ"}, {"v", "ㄏ"}, {"w", "ㄆ"}, {"x", "ㄈ"}, {"y", "ㄔ"},
    {"z", "ˋ"},  {"1", "˙"},  {"2", "ㄅ"}, {"3", "ㄉ"}, {"6", "ㄓ"},
    {"8", "ㄚ"}, {"9", "ㄞ"}, {"0", "ㄢ"}, {"4", "ㄧ"}, {";", "ㄤ"},
    {",", "ㄝ"}, {".", "ㄡ"}, {"/", "ㄥ"}, {"7", "ㄩ"}, {"5", "ㄨ"},
    {"-", "ㄦ"}, {" ", " "}};

/// 神通排列專用處理陣列。
inline static std::map<std::string, std::string> mapQwertyMiTAC = {
    {",", "ㄓ"}, {"-", "ㄦ"}, {".", "ㄔ"}, {"/", "ㄕ"}, {"0", "ㄥ"},
    {"1", "˙"},  {"2", "ˊ"},  {"3", "ˇ"},  {"4", "ˋ"},  {"5", "ㄞ"},
    {"6", "ㄠ"}, {"7", "ㄢ"}, {"8", "ㄣ"}, {"9", "ㄤ"}, {";", "ㄝ"},
    {"a", "ㄚ"}, {"b", "ㄅ"}, {"c", "ㄘ"}, {"d", "ㄉ"}, {"e", "ㄜ"},
    {"f", "ㄈ"}, {"g", "ㄍ"}, {"h", "ㄏ"}, {"i", "ㄟ"}, {"j", "ㄐ"},
    {"k", "ㄎ"}, {"l", "ㄌ"}, {"m", "ㄇ"}, {"n", "ㄋ"}, {"o", "ㄛ"},
    {"p", "ㄆ"}, {"q", "ㄑ"}, {"r", "ㄖ"}, {"s", "ㄙ"}, {"t", "ㄊ"},
    {"u", "ㄡ"}, {"v", "ㄩ"}, {"w", "ㄨ"}, {"x", "ㄒ"}, {"y", "ㄧ"},
    {"z", "ㄗ"}, {" ", " "},
};

/// 用以判定拼音鍵盤佈局的集合
inline static std::vector<MandarinParser> arrPinyinParsers = {
    ofHanyuPinyin,  ofSecondaryPinyin, ofYalePinyin,
    ofHualuoPinyin, ofUniversalPinyin, ofWadeGilesPinyin};

// MARK: - Phonabet to Hanyu-Pinyin Conversion Processing

/// 注音轉拼音，要求陰平必須是空格。
///
/// @param targetJoined 傳入的 String 對象物件。
inline static std::string cnvPhonaToHanyuPinyin(std::string targetJoined = "") {
  std::string strResult = std::move(targetJoined);
  for (std::vector<std::string> i : arrPhonaToHanyuPinyin) {
    replaceOccurrences(strResult, i[0], i[1]);
  }
  return strResult;
}

/// 漢語拼音數字標調式轉漢語拼音教科書格式，要求陰平必須是數字 1。
///
/// @param targetJoined 傳入的 String 對象物件。
inline static std::string cnvHanyuPinyinToTextBookStyle(
    std::string targetJoined) {
  std::string strResult = std::move(targetJoined);
  for (std::vector<std::string> i :
       arrHanyuPinyinTextbookStyleConversionTable) {
    replaceOccurrences(strResult, i[0], i[1]);
  }
  return strResult;
}

/// 該函式負責將注音轉為教科書印刷的方式（先寫輕聲）。
/// @param target 要拿來做轉換處理的讀音。
/// @returns 經過轉換處理的讀音鏈。
inline static std::string cnvPhonaToTextbookStyle(std::string target) {
  std::string result = target;
  if (stringInclusion(result, "˙")) {
    // 輕聲記號需要 pop_back() 兩次才可以徹底清除。
    result.pop_back();
    result.pop_back();
    result = "˙" + result;
  }
  return result;
}

/// 該函式用來恢復注音當中的陰平聲調，恢復之後會以「1」表示陰平。
/// @param target 要拿來做轉換處理的讀音。
/// @returns 經過轉換處理的讀音鏈。
inline static std::string restoreToneOneInPhona(std::string target) {
  std::string result = target;
  if (result.find("ˊ") == std::string::npos &&
      result.find("ˇ") == std::string::npos &&
      result.find("ˋ") == std::string::npos &&
      result.find("˙") == std::string::npos)
    result = result + "1";
  return result;
}

/// 該函式用來將漢語拼音轉為注音。
/// @param targetJoined 要轉換的漢語拼音內容，要求必須帶有 12345 數字標調。
/// @param newToneOne 對陰平指定新的標記。預設情況下該標記為空字串。
/// @returns 轉換結果。
inline static std::string cnvHanyuPinyinToPhona(std::string targetJoined = "",
                                                std::string newToneOne = "") {
  std::regex str_reg(".*[^A-Za-z0-9].*");
  std::smatch matchResult;
  if (stringInclusion(targetJoined, "_") ||
      !std::regex_match(targetJoined, matchResult, str_reg))
    return targetJoined;
  std::string strResult = std::move(targetJoined);
  std::vector<std::string> keyListHYPY;
  for (auto const& i : mapHanyuPinyin) keyListHYPY.push_back(i.first);
  std::sort(keyListHYPY.begin(), keyListHYPY.end(),
            [](const std::string& first, const std::string& second) {
              return first.size() > second.size();
            });

  std::vector<std::string> keyListIntonation;
  for (auto const& i : mapArayuruPinyinIntonation)
    keyListIntonation.push_back(i.first);
  std::sort(keyListIntonation.begin(), keyListIntonation.end(),
            [](const std::string& first, const std::string& second) {
              return first.size() > second.size();
            });

  for (auto i : keyListHYPY) {
    replaceOccurrences(strResult, i, mapHanyuPinyin[i]);
  }
  for (auto i : keyListIntonation) {
    replaceOccurrences(strResult, i,
                       i == "1" ? newToneOne : mapArayuruPinyinIntonation[i]);
  }
  return strResult;
}

// ========================================================================
// ======================== REAL THINGS BEGIN HERE ========================
// ========================================================================

// MARK: - Phonabet Structure

/// 注音符號型別。本身與字串差不多，但卻只能被設定成一個注音符號字符。
/// 然後會根據自身的 value 的內容值自動計算自身的 PhoneType 類型（聲介韻調假）。
/// 如果遇到被設為多個字符、或者字符不對的情況的話，value 會被清空、PhoneType
/// 會變成 null。 賦值時最好直接重新 init 且一直用 let 來初期化 Phonabet。 其實
/// value 對外只讀，對內的話另有 valueStorage 代為存儲內容。這樣比較安全一些。
struct Phonabet {
 public:
  PhoneType type = null;
  std::string value() { return strStorage; }
  bool isEmpty() { return value().empty(); }
  bool isValid() { return type != null; }

  ~Phonabet() { clear(); }
  /// 初期化，會根據傳入的 input 字串參數來自動判定自身的 PhoneType 類型屬性值。
  explicit Phonabet(std::string input = "") {
    strStorage = input;
    ensureType();
  }

  /// 自我清空內容。
  void clear() {
    strStorage.clear();
    type = null;
  };

  /// 自我變換資料值。
  ///
  /// @param strOf 要取代的內容。
  /// @param strWith 要取代成的內容。
  void selfReplace(std::string strOf, std::string strWith) {
    if (strStorage == strOf) {
      strStorage = strWith;
      ensureType();
    }
  }

 protected:
  std::string strStorage;

  /// 判定自身的 PhoneType 類型屬性值。
  void ensureType() {
    if (contains(allowedConsonants, strStorage)) {
      type = consonant;
    } else if (contains(allowedSemivowels, strStorage)) {
      type = semivowel;
    } else if (contains(allowedVowels, strStorage)) {
      type = vowel;
    } else if (contains(allowedIntonations, strStorage)) {
      type = intonation;
    } else {
      type = null;
      strStorage.clear();
    }
  }
};

// MARK: - Composer

class Composer {
 public:
  // 聲介韻調。寫這樣囉唆是為了直接在這裡初期化。
  Phonabet consonant = Phonabet(), semivowel = Phonabet(), vowel = Phonabet(),
           intonation = Phonabet();

  /// 為拉丁字母專用的組音區。
  std::string romajiBuffer;

  /// 注音排列種類。預設情況下是大千排列（Windows / macOS 預設注音排列）。
  MandarinParser parser = ofDachen;

  /// 是否對錯誤的注音讀音組合做出自動糾正處理。
  bool phonabetCombinationCorrectionEnabled;

  /// 內容值，會直接按照正確的順序拼裝自己的聲介韻調內容、再回傳。
  /// 注意：直接取這個參數的內容的話，陰平聲調會成為一個空格。
  /// 如果是要取不帶空格的注音的話，請使用「.getComposition()」而非「.Value」。
  std::string value() {
    return consonant.value() + semivowel.value() + vowel.value() +
           intonation.value();
  };

  /// 當前注拼槽是否處於拼音模式。
  bool isPinyinMode() { return parser >= 100; }

  /// 統計有效的聲介韻（調）個數。
  ///
  /// @param withIntonation 是否統計聲調。
  int count(bool withIntonation = false) {
    int result = (intonation.isValid() && withIntonation) ? 1 : 0;
    result += consonant.isValid() ? 1 : 0;
    result += semivowel.isValid() ? 1 : 0;
    result += vowel.isValid() ? 1 : 0;
    return result;
  }

  /// 與 value 類似，這個函式就是用來決定輸入法組字區內顯示的注音/拼音內容，
  /// 但可以指定是否輸出教科書格式（拼音的調號在字母上方、注音的輕聲寫在左側）。
  ///
  /// @param isHanyuPinyin 是否將輸出結果轉成漢語拼音。
  /// @param isTextBookStyle 是否將輸出的注音/拼音結果轉成教科書排版格式。
  std::string getComposition(bool isHanyuPinyin = false,
                             bool isTextBookStyle = false) {
    if (isHanyuPinyin) {  // 拼音輸出的場合
      std::string valReturnPinyin = cnvPhonaToHanyuPinyin(value());
      if (isTextBookStyle)
        valReturnPinyin = cnvHanyuPinyinToTextBookStyle(valReturnPinyin);
      return valReturnPinyin;
    } else {  // 注音輸出的場合
      std::string valReturnZhuyin = value();
      replaceOccurrences(valReturnZhuyin, " ", "");
      if (isTextBookStyle) {
        valReturnZhuyin = cnvPhonaToTextbookStyle(valReturnZhuyin);
      }
      // 下面這段不能砍，因為 Cpp 在執行上述步驟時會加上「\xCB」這個北七後綴，
      // 然後單元測試就會廢掉，因為單元測試那邊的 String Literal 並非以此結尾。
      if (valReturnZhuyin.back() == '\xCB') {
        valReturnZhuyin.pop_back();
      }
      return valReturnZhuyin;
    }
  }

  // 該函式僅用來獲取給 macOS InputMethod Kit 的內文組字區使用的顯示字串。
  ///
  /// @param isHanyuPinyin 是否將輸出結果轉成漢語拼音。
  std::string getInlineCompositionForDisplay(bool isHanyuPinyin = false) {
    if (!isPinyinMode()) return getComposition(isHanyuPinyin);
    std::string result;
    std::string toneReturned;
    switch (hashify(intonation.value().c_str())) {
      case hashify(" "):
        toneReturned = "1";
        break;
      case hashify("ˊ"):
        toneReturned = "2";
        break;
      case hashify("ˇ"):
        toneReturned = "3";
        break;
      case hashify("ˋ"):
        toneReturned = "4";
        break;
      case hashify("˙"):
        toneReturned = "5";
        break;
    }
    result = romajiBuffer + toneReturned;
    replaceOccurrences(result, "v", "ü");
    return result;
  }

  /// 注拼槽內容是否為空。
  bool isEmpty() {
    if (!isPinyinMode()) return intonation.isEmpty() && romajiBuffer.empty();
    return consonant.isEmpty() && semivowel.isEmpty() && vowel.isEmpty() &&
           intonation.isEmpty();
  }

  /// 注拼槽內容是否可唸。
  bool isPronounceable() {
    return !vowel.isEmpty() || !semivowel.isEmpty() || !consonant.isEmpty();
  }

  // MARK: 注拼槽對外處理函式.

  ~Composer() { clear(); }

  /// 初期化一個新的注拼槽。可以藉由 @input 參數指定初期已經傳入的按鍵訊號。
  /// 還可以在初期化時藉由 @arrange
  /// 參數來指定注音排列（預設為「.ofDachen」大千佈局）。
  ///
  /// @param input 傳入的 String 內容，用以處理單個字符。
  /// @param arrange 要使用的注音排列。
  /// @param correction 是否對錯誤的注音讀音組合做出自動糾正處理。
  explicit Composer(std::string input = "", MandarinParser arrange = ofDachen,
                    bool correction = false) {
    phonabetCombinationCorrectionEnabled = correction;
    romajiBuffer = "";
    ensureParser(arrange);
    receiveKey(input);
  }

  /// 清除自身的內容，就是將聲介韻調全部清空。
  /// 嚴格而言，「注音排列」這個屬性沒有需要清空的概念，只能用 ensureParser
  /// 參數變更之。
  void clear() {
    consonant.clear();
    semivowel.clear();
    vowel.clear();
    intonation.clear();
    romajiBuffer.clear();
  }

  /// 用於檢測「某個輸入字符訊號的合規性」的函式。
  ///
  /// 注意：回傳結果會受到當前注音排列 parser 屬性的影響。
  ///
  /// @param inputCharCode 傳入的 charCode 內容。
  [[nodiscard]] bool inputValidityCheck(char inputCharCode) {
    std::string inputKey(1, inputCharCode);
    return ((int)inputCharCode < 128) && inputValidityCheckStr(inputKey);
  }

  /// 用於檢測「某個輸入字符訊號的合規性」的函式。
  ///
  /// 注意：回傳結果會受到當前注音排列 parser 屬性的影響。
  ///
  /// @param charStr 傳入的字元（String）。
  [[nodiscard]] bool inputValidityCheckStr(std::string charStr) {
    switch (parser) {
      case ofDachen:
        return mapQwertyDachen.find(charStr) != mapQwertyDachen.end();
      case ofDachen26:
        return mapDachenCP26StaticKeys.find(charStr) !=
               mapDachenCP26StaticKeys.end();
      case ofETen:
        return mapQwertyETenTraditional.find(charStr) !=
               mapQwertyETenTraditional.end();
      case ofHsu:
        return mapHsuStaticKeys.find(charStr) != mapHsuStaticKeys.end();
      case ofETen26:
        return mapETen26StaticKeys.find(charStr) != mapETen26StaticKeys.end();
      case ofIBM:
        return mapQwertyIBM.find(charStr) != mapQwertyIBM.end();
      case ofMiTAC:
        return mapQwertyMiTAC.find(charStr) != mapQwertyMiTAC.end();
      case ofSeigyou:
        return mapSeigyou.find(charStr) != mapSeigyou.end();
      case ofFakeSeigyou:
        return mapFakeSeigyou.find(charStr) != mapFakeSeigyou.end();
      case ofStarlight:
        return mapStarlightStaticKeys.find(charStr) !=
               mapStarlightStaticKeys.end();
      case ofAlvinLiu:
        return mapAlvinLiuStaticKeys.find(charStr) !=
               mapAlvinLiuStaticKeys.end();
      case ofWadeGilesPinyin:
        return stringInclusion(mapWadeGilesPinyinKeys, charStr);
      case ofHanyuPinyin:
      case ofSecondaryPinyin:
      case ofYalePinyin:
      case ofHualuoPinyin:
      case ofUniversalPinyin:
        return stringInclusion(mapArayuruPinyin, charStr);
    }
    return false;
  }

  /// 自我變換單個注音資料值。
  /// @param strOf 要取代的內容。
  /// @param strWith 要取代成的內容。
  void fixValue(std::string strOf, std::string strWith) {
    Phonabet theOld = Phonabet(strOf);
    if (consonant.value() == strOf)
      consonant.clear();
    else if (semivowel.value() == strOf)
      semivowel.clear();
    else if (vowel.value() == strOf)
      vowel.clear();
    else if (intonation.value() == strOf)
      intonation.clear();
    else
      return;
    receiveKeyFromPhonabet(strWith);
  }

  void updateRomajiBuffer() {
    romajiBuffer = Tekkon::cnvPhonaToHanyuPinyin(
        consonant.value() + semivowel.value() + vowel.value());
  }

  /// 接受傳入的按鍵訊號時的處理，處理對象為 String。
  /// 另有同名函式可處理 UniChar 訊號。
  ///
  /// 如果是諸如複合型注音排列的話，翻譯結果有可能為空，但翻譯過程已經處理好聲介韻調分配了。
  ///
  /// @param input 傳入的 String 內容。
  void receiveKey(std::string input) {
    if (!isPinyinMode()) {
      receiveKeyFromPhonabet(translate(input));
      return;
    }
    int maxCount;
    if (mapArayuruPinyinIntonation.count(input)) {
      std::string theTone = mapArayuruPinyinIntonation[input];
      intonation = Phonabet(theTone);
    } else {
      // 為了防止 RomajiBuffer 越敲越長帶來算力負擔，
      // 這裡讓它在要溢出時自動丟掉最早輸入的音頭。
      maxCount = (parser == ofWadeGilesPinyin) ? 7 : 6;
      if (romajiBuffer.length() > maxCount - 1) {
        romajiBuffer.erase(0, 1);
      }
      std::string romajiBufferBackup = romajiBuffer + input;
      receiveSequence(romajiBufferBackup, true);
      romajiBuffer = romajiBufferBackup;
    }
  }

  /// 接受傳入的按鍵訊號時的處理，處理對象為 UniChar。
  /// 其實也就是先將 UniChar 轉為 String 再交給某個同名異參的函式來處理而已。
  ///
  /// 如果是諸如複合型注音排列的話，翻譯結果有可能為空，但翻譯過程已經處理好聲介韻調分配了。
  ///
  /// @param input 傳入的 UniChar 內容。
  void receiveKey(char input) { receiveKey((charToString(input))); };

  /// 接受傳入的按鍵訊號時的處理，處理對象為單個注音符號。
  /// 主要就是將注音符號拆分辨識且分配到正確的貯存位置而已。
  ///
  /// @param phonabet 傳入的單個注音符號字串。
  void receiveKeyFromPhonabet(std::string phonabet = "") {
    Phonabet thePhone = Phonabet(phonabet);
    if (phonabetCombinationCorrectionEnabled) {
      switch (hashify(phonabet.c_str())) {
        case hashify("ㄧ"):
        case hashify("ㄩ"):
          if (vowel.value() == "ㄜ") vowel = Phonabet("ㄝ");
          break;
        case hashify("ㄜ"):
          if (semivowel.value() == "ㄨ") semivowel = Phonabet("ㄩ");
          if (semivowel.value() == "ㄧ" || semivowel.value() == "ㄩ")
            thePhone = Phonabet("ㄝ");
          break;
        case hashify("ㄝ"):
          if (semivowel.value() == "ㄨ") semivowel = Phonabet("ㄩ");
          break;
        case hashify("ㄛ"):
        case hashify("ㄥ"):
          if ((consonant.value() == "ㄅ" || consonant.value() == "ㄆ" ||
               consonant.value() == "ㄇ" || consonant.value() == "ㄈ") &&
              semivowel.value() == "ㄨ")
            semivowel.clear();
          break;
        case hashify("ㄟ"):
          if ((consonant.value() == "ㄋ" || consonant.value() == "ㄌ") &&
              (semivowel.value() == "ㄨ"))
            semivowel.clear();
          break;
        case hashify("ㄨ"):
          if ((consonant.value() == "ㄅ" || consonant.value() == "ㄆ" ||
               consonant.value() == "ㄇ" || consonant.value() == "ㄈ") &&
              (vowel.value() == "ㄛ" || vowel.value() == "ㄥ"))
            vowel.clear();
          if ((consonant.value() == "ㄋ" || consonant.value() == "ㄌ") &&
              (vowel.value() == "ㄟ"))
            vowel.clear();
          if (vowel.value() == "ㄜ") vowel = Phonabet("ㄝ");
          if (vowel.value() == "ㄝ") thePhone = Phonabet("ㄩ");
          break;
        case hashify("ㄅ"):
        case hashify("ㄆ"):
        case hashify("ㄇ"):
        case hashify("ㄈ"):
          if (semivowel.value() + vowel.value() == "ㄨㄛ" ||
              semivowel.value() + vowel.value() == "ㄨㄥ")
            semivowel.clear();
          break;
        default:
          break;
      }
      if ((thePhone.type == PhoneType::vowel ||
           thePhone.type == PhoneType::intonation) &&
          (consonant.value() == "ㄓ" || consonant.value() == "ㄔ" ||
           consonant.value() == "ㄕ" || consonant.value() == "ㄗ" ||
           consonant.value() == "ㄘ" || consonant.value() == "ㄙ")) {
        switch (hashify(semivowel.value().c_str())) {
          case hashify("ㄧ"):
            semivowel.clear();
            break;
          case hashify("ㄩ"):
            if (consonant.value() == "ㄓ" || consonant.value() == "ㄗ")
              consonant = Phonabet("ㄐ");
            if (consonant.value() == "ㄔ" || consonant.value() == "ㄘ")
              consonant = Phonabet("ㄑ");
            if (consonant.value() == "ㄕ" || consonant.value() == "ㄙ")
              consonant = Phonabet("ㄒ");
            break;
          default:
            break;
        }
      }
    }

    switch (thePhone.type) {
      case PhoneType::consonant:
        consonant = thePhone;
        break;
      case PhoneType::semivowel:
        semivowel = thePhone;
        break;
      case PhoneType::vowel:
        vowel = thePhone;
        break;
      case PhoneType::intonation:
        intonation = thePhone;
        break;
      default:
        break;
    }
    updateRomajiBuffer();
  }

  /// 處理一連串的按鍵輸入、且返回被處理之後的注音（陰平為空格）。
  ///
  /// @param givenSequence 傳入的 String 內容，用以處理一整串擊鍵輸入。
  /// @param isRomaji 若輸入的字串是基於西文字母的各種拼音的話，請啟用此選項。
  std::string receiveSequence(std::string givenSequence = "",
                              bool isRomaji = false) {
    clear();
    if (!isRomaji) {
      for (char key : givenSequence) {
        receiveKey(key);
      }
      return value();
    }
    std::vector<std::string> dictResult = {};
    switch (parser) {
      case ofHanyuPinyin:
        if (mapHanyuPinyin.count(givenSequence))
          dictResult = splitByCodepoint(mapHanyuPinyin[givenSequence]);
        break;
      case ofSecondaryPinyin:
        if (mapSecondaryPinyin.count(givenSequence))
          dictResult = splitByCodepoint(mapSecondaryPinyin[givenSequence]);
        break;
      case ofYalePinyin:
        if (mapYalePinyin.count(givenSequence))
          dictResult = splitByCodepoint(mapYalePinyin[givenSequence]);
        break;
      case ofHualuoPinyin:
        if (mapHualuoPinyin.count(givenSequence))
          dictResult = splitByCodepoint(mapHualuoPinyin[givenSequence]);
        break;
      case ofUniversalPinyin:
        if (mapUniversalPinyin.count(givenSequence))
          dictResult = splitByCodepoint(mapUniversalPinyin[givenSequence]);
        break;
      case ofWadeGilesPinyin:
        if (mapWadeGilesPinyin.count(givenSequence))
          dictResult = splitByCodepoint(mapWadeGilesPinyin[givenSequence]);
        break;
      default:
        break;
    }
    for (std::string phonabet : dictResult) receiveKeyFromPhonabet(phonabet);
    return value();
  }

  /// 專門用來響應使用者摁下 BackSpace 按鍵時的行為。
  /// 刪除順序：調、韻、介、聲。
  ///
  /// 基本上就是按順序從游標前方開始往後刪。
  void doBackSpace() {
    if (isPinyinMode() && !romajiBuffer.empty()) {
      if (!intonation.isEmpty()) {
        intonation.clear();
      } else {
        romajiBuffer.pop_back();
      }
    } else if (!intonation.isEmpty()) {
      intonation.clear();
    } else if (!vowel.isEmpty()) {
      vowel.clear();
    } else if (!semivowel.isEmpty()) {
      semivowel.clear();
    } else if (!consonant.isEmpty()) {
      consonant.clear();
    }
  }

  /// 用來檢測是否有調號的函式，預設情況下不判定聲調以外的內容的存無。
  ///
  /// @param withNothingElse 追加判定「槽內是否僅有調號」。
  bool hasIntonation(bool withNothingElse = false) {
    return withNothingElse ? (!intonation.isEmpty() && vowel.isEmpty() &&
                              semivowel.isEmpty() && consonant.isEmpty())
                           : !intonation.isEmpty();
  }

  /// 設定該 Composer 處於何種鍵盤排列分析模式。
  ///
  /// @param arrange 給該注拼槽指定注音排列。
  void ensureParser(MandarinParser arrange) { parser = arrange; };

  /// 拿取用來進行索引檢索用的注音字串。
  ///
  /// 如果輸入法的辭典索引是漢語拼音的話，你可能用不上這個函式。
  /// @remark 警告：該字串結果不能為空，否則組字引擎會炸。
  /// @param pronounceableOnly 是否可以唸出。
  std::string phonabetKeyForQuery(bool pronounceableOnly) {
    std::string readingKey = getComposition();
    bool validKeyAvailable = false;
    if (!isPinyinMode()) {
      validKeyAvailable =
          pronounceableOnly ? isPronounceable() : !readingKey.empty();
    } else {
      validKeyAvailable = isPronounceable();
    }
    return validKeyAvailable ? readingKey : "";
  }

 protected:
  // MARK: - Parser Processings

  // 注拼槽對內處理用函式都在這一小節。

  /// 根據目前的注音排列設定來翻譯傳入的 String 訊號。
  ///
  /// 倚天/許氏鍵盤/酷音大千二十六鍵的處理函式會代為處理分配過程，此時回傳結果可能為空字串。
  ///
  /// @param key 傳入的 String 訊號。
  std::string translate(std::string key) {
    if (isPinyinMode()) return "";
    switch (parser) {
      case ofDachen:
        return mapQwertyDachen.count(key) ? mapQwertyDachen[key] : "";
      case ofDachen26:
        return handleDachen26(key);
      case ofETen:
        return mapQwertyETenTraditional.count(key)
                   ? mapQwertyETenTraditional[key]
                   : "";
      case ofHsu:
        return handleHsu(key);
      case ofETen26:
        return handleETen26(key);
      case ofIBM:
        return mapQwertyIBM.count(key) ? mapQwertyIBM[key] : "";
      case ofMiTAC:
        return mapQwertyMiTAC.count(key) ? mapQwertyMiTAC[key] : "";
      case ofSeigyou:
        return mapSeigyou.count(key) ? mapSeigyou[key] : "";
      case ofFakeSeigyou:
        return mapFakeSeigyou.count(key) ? mapFakeSeigyou[key] : "";
      case ofStarlight:
        return handleStarlight(key);
      case ofAlvinLiu:
        return handleAlvinLiu(key);
      default:
        break;
    }
    return "";
  }

  /// 倚天忘形注音排列是複合注音排列，需要單獨處理。
  ///
  /// 回傳結果是空字串的話，不要緊，因為該函式內部已經處理過分配過程了。
  ///
  /// @param key 傳入的 std::string 訊號。
  std::string handleETen26(std::string key) {
    std::string strReturn =
        (mapETen26StaticKeys.count(key)) ? mapETen26StaticKeys[key] : "";

    std::string keysToHandleHere = "dfhjklmnpqtw";

    switch (hashify(key.c_str())) {
      case hashify("d"):
        if (isPronounceable()) strReturn = ("˙");
        break;
      case hashify("f"):
        if (isPronounceable()) strReturn = ("ˊ");
        break;
      case hashify("j"):
        if (isPronounceable()) strReturn = ("ˇ");
        break;
      case hashify("k"):
        if (isPronounceable()) strReturn = ("ˋ");
        break;
      case hashify("e"):
        if (consonant.value() == "ㄍ") consonant = Phonabet("ㄑ");
        break;
      case hashify("p"):
        if (!consonant.isEmpty() || semivowel.value() == "ㄧ") strReturn = "ㄡ";
        break;
      case hashify("h"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄦ");
        break;
      case hashify("l"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄥ");
        break;
      case hashify("m"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄢ");
        break;
      case hashify("n"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄣ");
        break;
      case hashify("q"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄟ");
        break;
      case hashify("t"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄤ");
        break;
      case hashify("w"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄝ");
        break;
      default:
        break;
    }

    if (stringInclusion(keysToHandleHere, key))
      receiveKeyFromPhonabet(strReturn);

    // 處理公共特殊情形。
    commonFixWhenHandlingDynamicArrangeInputs(Phonabet(strReturn));

    if (stringInclusion("dfjk ", key) && count() == 1) {
      fixValue("ㄆ", "ㄡ");
      fixValue("ㄇ", "ㄢ");
      fixValue("ㄊ", "ㄤ");
      fixValue("ㄋ", "ㄣ");
      fixValue("ㄌ", "ㄥ");
      fixValue("ㄏ", "ㄦ");
    }

    // 後置修正
    if (value() == "ㄍ˙") consonant = Phonabet("ㄑ");

    // 這些按鍵在上文處理過了，就不要再回傳了。
    if (stringInclusion(keysToHandleHere, key)) strReturn = "";

    // 回傳結果是空字串的話，不要緊，因為上文已經代處理過分配過程了。
    return strReturn;
  }

  /// 許氏注音排列是複合注音排列，需要單獨處理。
  ///
  /// 回傳結果是空的話，不要緊，因為該函式內部已經處理過分配過程了。
  ///
  /// @param key 傳入的 std::string 訊號。
  std::string handleHsu(std::string key) {
    std::string strReturn =
        (mapHsuStaticKeys.count(key)) ? mapHsuStaticKeys[key] : "";

    std::string keysToHandleHere = "acdefghjklmns";

    switch (hashify(key.c_str())) {
      case hashify("d"):
        if (isPronounceable()) strReturn = ("ˊ");
        break;
      case hashify("f"):
        if (isPronounceable()) strReturn = ("ˇ");
        break;
      case hashify("s"):
        if (isPronounceable()) strReturn = ("˙");
        break;
      case hashify("j"):
        if (isPronounceable()) strReturn = ("ˋ");
        break;
      case hashify("a"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄟ");
        break;
      case hashify("v"):
        if (!semivowel.isEmpty()) strReturn = ("ㄑ");
        break;
      case hashify("c"):
        if (!semivowel.isEmpty()) strReturn = ("ㄒ");
        break;
      case hashify("e"):
        if (!semivowel.isEmpty()) strReturn = ("ㄝ");
        break;
      case hashify("g"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄜ");
        break;
      case hashify("h"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄛ");
        break;
      case hashify("k"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄤ");
        break;
      case hashify("m"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄢ");
        break;
      case hashify("n"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄣ");
        break;
      case hashify("l"):
        if (value().empty() && !consonant.isEmpty() && !semivowel.isEmpty())
          strReturn = ("ㄦ");
        else if (consonant.isEmpty() && semivowel.isEmpty())
          strReturn = ("ㄌ");
        else
          strReturn = ("ㄥ");
        break;
      default:
        break;
    }

    if (stringInclusion(keysToHandleHere, key))
      receiveKeyFromPhonabet(strReturn);

    // 處理公共特殊情形。
    commonFixWhenHandlingDynamicArrangeInputs(Phonabet(strReturn));

    if (stringInclusion("dfjs ", key) && count() == 1) {
      fixValue("ㄒ", "ㄕ");
      fixValue("ㄍ", "ㄜ");
      fixValue("ㄋ", "ㄣ");
      fixValue("ㄌ", "ㄦ");
      fixValue("ㄎ", "ㄤ");
      fixValue("ㄇ", "ㄢ");
      fixValue("ㄐ", "ㄓ");
      fixValue("ㄑ", "ㄔ");
      fixValue("ㄒ", "ㄕ");
      fixValue("ㄏ", "ㄛ");
    }

    // 後置修正
    if (value() == "ㄔ˙") consonant = Phonabet("ㄑ");

    // 這些按鍵在上文處理過了，就不要再回傳了。
    if (stringInclusion(keysToHandleHere, key)) strReturn = "";

    // 回傳結果是空的話，不要緊，因為上文已經代處理過分配過程了。
    return strReturn;
  }

  /// 星光注音排列是複合注音排列，需要單獨處理。
  ///
  /// 回傳結果是空的話，不要緊，因為該函式內部已經處理過分配過程了。
  ///
  /// @param key 傳入的 std::string 訊號。
  std::string handleStarlight(std::string key) {
    std::string strReturn =
        (mapStarlightStaticKeys.count(key)) ? mapStarlightStaticKeys[key] : "";

    std::string keysToHandleHere = "efgklmnt";

    switch (hashify(key.c_str())) {
      case hashify("e"):
        if (semivowel.value() == "ㄧ" || semivowel.value() == "ㄩ")
          strReturn = "ㄝ";
        break;
      case hashify("f"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄠ";
        break;
      case hashify("g"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄥ";
        break;
      case hashify("k"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄤ";
        break;
      case hashify("l"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄦ";
        break;
      case hashify("m"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄢ";
        break;
      case hashify("n"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄣ";
        break;
      case hashify("t"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄟ";
        break;
      default:
        break;
    }

    if (stringInclusion(keysToHandleHere, key))
      receiveKeyFromPhonabet(strReturn);

    // 處理公共特殊情形。
    commonFixWhenHandlingDynamicArrangeInputs(Phonabet(strReturn));

    if (stringInclusion("67890 ", key) && count() == 1) {
      fixValue("ㄈ", "ㄠ");
      fixValue("ㄍ", "ㄥ");
      fixValue("ㄎ", "ㄤ");
      fixValue("ㄌ", "ㄦ");
      fixValue("ㄇ", "ㄢ");
      fixValue("ㄋ", "ㄣ");
      fixValue("ㄊ", "ㄟ");
    }

    // 這些按鍵在上文處理過了，就不要再回傳了。
    if (stringInclusion(keysToHandleHere, key)) strReturn = "";

    // 回傳結果是空的話，不要緊，因為上文已經代處理過分配過程了。
    return strReturn;
  }

  /// 酷音大千二十六鍵注音排列是複合注音排列，需要單獨處理。
  ///
  /// 回傳結果是空的話，不要緊，因為該函式內部已經處理過分配過程了。
  ///
  /// @param key 傳入的 std::string 訊號。
  std::string handleDachen26(std::string key) {
    std::string strReturn = (mapDachenCP26StaticKeys.count(key))
                                ? mapDachenCP26StaticKeys[key]
                                : "";

    switch (hashify(key.c_str())) {
      case (hashify("e")):
        if (isPronounceable()) strReturn = ("ˊ");
        break;
      case hashify("r"):
        if (isPronounceable()) strReturn = ("ˇ");
        break;
      case hashify("d"):
        if (isPronounceable()) strReturn = ("ˋ");
        break;
      case hashify("y"):
        if (isPronounceable()) strReturn = ("˙");
        break;
      case hashify("b"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = ("ㄝ");
        break;
      case hashify("i"):
        if (vowel.isEmpty() || vowel.value() == "ㄞ") strReturn = ("ㄛ");
        break;
      case hashify("l"):
        if (vowel.isEmpty() || vowel.value() == "ㄤ") strReturn = ("ㄠ");
        break;
      case hashify("n"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) {
          if (value() == "ㄙ") consonant.clear();
          strReturn = ("ㄥ");
        }
        break;
      case hashify("o"):
        if (vowel.isEmpty() || vowel.value() == "ㄢ") strReturn = ("ㄟ");
        break;
      case hashify("p"):
        if (vowel.isEmpty() || vowel.value() == "ㄦ") strReturn = ("ㄣ");
        break;
      case hashify("q"):
        if (consonant.isEmpty() || consonant.value() == "ㄅ")
          strReturn = ("ㄆ");
        break;
      case hashify("t"):
        if (consonant.isEmpty() || consonant.value() == "ㄓ")
          strReturn = ("ㄔ");
        break;
      case hashify("w"):
        if (consonant.isEmpty() || consonant.value() == "ㄉ")
          strReturn = ("ㄊ");
        break;
      case hashify("m"):
        if (semivowel.value() == "ㄩ" && vowel.value() != "ㄡ") {
          semivowel.clear();
          strReturn = ("ㄡ");
        } else if (semivowel.value() != "ㄩ" && vowel.value() == "ㄡ") {
          vowel.clear();
          strReturn = ("ㄩ");
        } else if (!semivowel.isEmpty())
          strReturn = ("ㄡ");
        else {
          strReturn = (consonant.value() == "ㄐ" || consonant.value() == "ㄑ" ||
                       consonant.value() == "ㄒ")
                          ? "ㄩ"
                          : "ㄡ";
        }
        break;
      case hashify("u"):
        if (semivowel.value() == "ㄧ" && vowel.value() != "ㄚ") {
          semivowel.clear();
          strReturn = ("ㄚ");
        } else if (semivowel.value() != "ㄧ" && vowel.value() == "ㄚ")
          strReturn = ("ㄧ");
        else if (semivowel.value() == "ㄧ" && vowel.value() == "ㄚ") {
          semivowel.clear();
          vowel.clear();
        } else if (!semivowel.isEmpty())
          strReturn = ("ㄚ");
        else
          strReturn = ("ㄧ");
        break;
      default:
        break;
    }

    // 回傳結果是空的話，不要緊，因為上文已經代處理過分配過程了。
    return strReturn;
  }

  /// 劉氏擬音注音排列是複合注音排列，需要單獨處理。
  ///
  /// 回傳結果是空的話，不要緊，因為該函式內部已經處理過分配過程了。
  /// @remark 該處理兼顧了「原旨排列方案」與「微軟新注音相容排列方案」。
  /// @param key 傳入的 std::string 訊號。
  std::string handleAlvinLiu(std::string key) {
    std::string strReturn =
        (mapAlvinLiuStaticKeys.count(key)) ? mapAlvinLiuStaticKeys[key] : "";

    // 前置處理專有特殊情形。
    if (strReturn != "ㄦ" && !vowel.isEmpty()) fixValue("ㄦ", "ㄌ");

    std::string keysToHandleHere = "dfjlegnhkbmc";

    switch (hashify(key.c_str())) {
      case hashify("d"):
        if (isPronounceable()) strReturn = ("˙");
        break;
      case hashify("f"):
        if (isPronounceable()) strReturn = ("ˊ");
        break;
      case hashify("j"):
        if (isPronounceable()) strReturn = ("ˇ");
        break;
      case hashify("l"):
        if (isPronounceable()) strReturn = ("ˋ");
        break;
      case hashify("e"):
        if (semivowel.value() == "ㄧ" || semivowel.value() == "ㄩ")
          strReturn = "ㄝ";
        break;
      case hashify("g"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄤ";
        break;
      case hashify("n"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄣ";
        break;
      case hashify("h"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄞ";
        break;
      case hashify("k"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄟ";
        break;
      case hashify("b"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄢ";
        break;
      case hashify("m"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄥ";
        break;
      case hashify("c"):
        if (!consonant.isEmpty() || !semivowel.isEmpty()) strReturn = "ㄝ";
        break;
      default:
        break;
    }

    if (stringInclusion(keysToHandleHere, key))
      receiveKeyFromPhonabet(strReturn);

    // 處理公共特殊情形。
    commonFixWhenHandlingDynamicArrangeInputs(Phonabet(strReturn));

    if (stringInclusion("dfjl ", key) && count() == 1) {
      fixValue("ㄑ", "ㄔ");
      fixValue("ㄊ", "ㄦ");
      fixValue("ㄍ", "ㄤ");
      fixValue("ㄏ", "ㄞ");
      fixValue("ㄐ", "ㄓ");
      fixValue("ㄎ", "ㄟ");
      fixValue("ㄌ", "ㄦ");
      fixValue("ㄒ", "ㄕ");
      fixValue("ㄅ", "ㄢ");
      fixValue("ㄋ", "ㄣ");
      fixValue("ㄇ", "ㄥ");
    }

    // 這些按鍵在上文處理過了，就不要再回傳了。
    if (stringInclusion(keysToHandleHere, key)) strReturn = "";

    // 回傳結果是空的話，不要緊，因為上文已經代處理過分配過程了。
    return strReturn;
  }

  /// 所有動態注音排列都會用到的共用糾錯處理步驟。
  /// @param incomingPhonabet 傳入的注音 Phonabet。
  void commonFixWhenHandlingDynamicArrangeInputs(Phonabet incomingPhonabet) {
    switch (incomingPhonabet.type) {
      case PhoneType::semivowel:
        switch (hashify(consonant.value().c_str())) {
          case hashify("ㄍ"):
            switch (hashify(incomingPhonabet.value().c_str())) {
              // 這裡不處理「ㄍㄧ」到「ㄑㄧ」的轉換，因為只有倚天26需要處理這個。
              // case hashify("ㄧ"): consonant = Phonabet("ㄑ"); break;  // ㄑㄧ
              case hashify("ㄨ"):
                consonant = Phonabet("ㄍ");
                break;  // ㄍㄨ
              case hashify("ㄩ"):
                consonant = Phonabet("ㄑ");
                break;  // ㄑㄩ
              default:
                break;
            }
            break;
          case hashify("ㄓ"):
            switch (hashify(incomingPhonabet.value().c_str())) {
              case hashify("ㄧ"):
                consonant = Phonabet("ㄐ");
                break;  // ㄐㄧ
              case hashify("ㄨ"):
                consonant = Phonabet("ㄓ");
                break;  // ㄓㄨ
              case hashify("ㄩ"):
                consonant = Phonabet("ㄐ");
                break;  // ㄐㄩ
              default:
                break;
            }
            break;
          case hashify("ㄔ"):
            switch (hashify(incomingPhonabet.value().c_str())) {
              case hashify("ㄧ"):
                consonant = Phonabet("ㄑ");
                break;  // ㄐㄧ
              case hashify("ㄨ"):
                consonant = Phonabet("ㄔ");
                break;  // ㄓㄨ
              case hashify("ㄩ"):
                consonant = Phonabet("ㄑ");
                break;  // ㄐㄩ
              default:
                break;
            }
            break;
          case hashify("ㄕ"):
            switch (hashify(incomingPhonabet.value().c_str())) {
              case hashify("ㄧ"):
                consonant = Phonabet("ㄒ");
                break;  // ㄒㄧ
              case hashify("ㄨ"):
                consonant = Phonabet("ㄕ");
                break;  // ㄕㄨ
              case hashify("ㄩ"):
                consonant = Phonabet("ㄒ");
                break;  // ㄒㄩ
              default:
                break;
            }
            break;
          default:
            break;
        }
        if (incomingPhonabet.value() == "ㄨ") {
          fixValue("ㄐ", "ㄓ");
          fixValue("ㄑ", "ㄔ");
          fixValue("ㄒ", "ㄕ");
        }
        break;
      case PhoneType::vowel:
        if (semivowel.isEmpty()) {
          fixValue("ㄐ", "ㄓ");
          fixValue("ㄑ", "ㄔ");
          fixValue("ㄒ", "ㄕ");
        }
        break;
      default:
        break;
    }
  }
};

}  // namespace Tekkon

#endif
