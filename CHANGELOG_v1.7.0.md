# TekkonCC v1.7.0 變更日誌

本文件總結了從 Swift Tekkon v1.7.0 移植到 TekkonCC 的變更內容。

## 重大變更

### Unicode 純量遷移（char32_t）

注音符元的核心資料型別已從 `std::string` 遷移至 `char32_t`（Unicode 純量）：

#### 1. 允許的注音符元常數

**變更前（v1.6.0）：**
```cpp
inline static std::vector<std::string> allowedConsonants = {
    "ㄅ", "ㄆ", "ㄇ", ...
};
```

**變更後（v1.7.0）：**
```cpp
inline static std::vector<char32_t> allowedConsonants = {
    U'ㄅ', U'ㄆ', U'ㄇ', ...
};
```

此變更適用於：
- `allowedConsonants`（允許的聲母）
- `allowedSemivowels`（允許的介母）
- `allowedVowels`（允許的韻母）
- `allowedIntonations`（允許的聲調）
- `allowedPhonabets`（允許的注音符元）

#### 2. Phonabet 結構

**變更前（v1.6.0）：**
- 以 `std::string strStorage` 儲存值
- 建構子：`Phonabet(std::string input = "")`
- 存取：`value()` 回傳內部字串

**變更後（v1.7.0）：**
- 以 `char32_t scalarValue` 儲存值
- 新增建構子：`Phonabet(char32_t input)`
- 存取方式：
  - `value()` 回傳 UTF-8 `std::string`（相容性用途）
  - `scalar()` 回傳 `char32_t`（直接存取）
- 舊版字串建構子仍可使用以保持向下相容

#### 3. 新增方法

新增 `receiveKeyFromPhonabet(char32_t phonabet)` 以實現高效的 Unicode 純量處理。

字串版本的 `receiveKeyFromPhonabet(std::string phonabet)` 仍保留以維持向下相容，並在內部轉換為 char32_t。

## API 設計

### 唯讀屬性

依循 Swift v1.7.0 的 `public internal(set)` 屬性模式，以下 Composer 成員現已標註為唯讀（雖然在 C++ 中仍為 public）：

- `consonant`（聲母）、`semivowel`（介母）、`vowel`（韻母）、`intonation`（聲調）
- `romajiBuffer`（羅馬拼音緩衝區）
- `parser`（解析器）

**建議用法：**
- ✅ 直接讀取這些屬性
- ❌ 避免直接修改
- ✅ 使用 `receiveKey()`、`receiveSequence()` 或 `clear()` 來修改狀態
- ✅ 使用 `ensureParser()` 來變更解析器

## 輔助函式

新增 `char32ToString(char32_t scalar)` 以將 Unicode 純量轉換為 UTF-8 字串。

## 相容性

### 向下相容

- 所有既有的字串基礎 API 仍可正常運作
- 測試無需變更
- 使用字串基礎 API 的外部程式碼將繼續運作

### 效能優勢

- 減少字串比較與轉換
- 單一 Unicode 碼位比較取代多位元組字串操作
- 更有效率的注音符元驗證

## 遷移指南

### 對於函式庫使用者

若您透過公開 API 使用 TekkonCC：
- **無需變更** - 向下相容的字串 API 均獲保留
- 在處理個別注音符元時，建議使用新的 char32_t API 以獲得更佳效能

### 對於貢獻者

在處理注音符元內部時：
- 使用 `char32_t`（例如：`U'ㄅ'`）而非 `std::string`（例如：`"ㄅ"`）
- 使用 `.scalar()` 從 Phonabet 取得 char32_t 值
- 使用 `.value()` 取得 UTF-8 字串表示

## 測試

所有 16 個既有單元測試無需修改即可通過：
- 4 個基礎測試
- 6 個中階測試（拼音處理）
- 6 個進階測試（鍵盤配置）

## 新增功能（v1.7.0 完整移植）

### PinyinTrie 與 Chopper

實作完整的 PinyinTrie 類別以進行智慧拼音處理：

**功能：**
- `PinyinTrie` 類別：使用字首樹結構進行高效的拼音轉注音映射
- `chop()` 方法：智慧切割連續簡拼字串（例如："shjdaz" → ["sh", "j", "da", "z"]）
- `deductChoppedPinyinToZhuyin()`：將切割後的拼音片段轉換為注音候選（例如：["b", "yue", "z"] → ["ㄅ", "ㄩㄝ", "ㄓ&ㄗ"]）
- `insert()`、`search()`：核心字首樹操作以進行拼音查找
- 支援所有拼音解析器（漢語拼音、國音二式、耶魯拼音、華羅拼音、通用拼音、韋氏拼音）

**使用情境：**
- 智慧/模糊拼音輸入
- 簡拼支援
- 拼音自動切分

### 測試資料整合

**集中化測試資料：**
- 建立 `Tests/TestAssets_Tekkon/TekkonTestData.hh` 並包含 1486 行測試資料
- 從 Swift v1.7.0 移植完整的 testTable4DynamicLayouts
- 使用 C++ 原始字串常值以利維護

**測試結構重組：**
- `GTests/TekkonTests_Arrangements.cc`：使用集中化資料測試鍵盤配置
- `GTests/TekkonTests_Pinyin.cc`：測試 PinyinTrie 與 Chopper 功能
- 從集中化資料動態產生測試（符合 Swift v1.7.0 的作法）
- 模組化組織以利維護

**測試涵蓋範圍：**
- 23 個測試（從 16 個增加）
  - 4 個基礎測試
  - 6 個中階測試（拼音）
  - 6 個進階測試（鍵盤配置）
  - 2 個配置測試（使用集中化資料）
  - 5 個拼音/字首樹測試（新功能）
- 100% 測試通過率

## 效能優勢

- 單一 Unicode 碼位比較取代多位元組字串操作
- 記憶體配置減少：char32_t（4 位元組）對比 std::string（在大多數平台上為 24 位元組以上）
- 型別安全：編譯期間保證注音符元有效性
- 更快的注音符元驗證：在 `ensureType()` 中直接進行 char32_t 比較

## 遷移範例

```cpp
// 舊程式碼仍可運作：
Phonabet p1("ㄅ");
composer.receiveKeyFromPhonabet("ㄅ");

// 新的高效 API 可用：
Phonabet p2(U'ㄅ');  // 直接使用 Unicode 純量
composer.receiveKeyFromPhonabet(U'ㄅ');  // 更快
char32_t c = p2.scalar();  // 直接存取

// 智慧拼音使用新的 PinyinTrie：
PinyinTrie trie(ofHanyuPinyin);
auto chopped = trie.chop("shjdaz");  // 智慧切分
auto zhuyin = trie.deductChoppedPinyinToZhuyin(chopped);
```

**既有使用者無需變更程式碼**。新 API 為選用功能，可用於效能提升與新功能。
