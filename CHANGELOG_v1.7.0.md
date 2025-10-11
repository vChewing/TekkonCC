# TekkonCC v1.7.0 Changes

This document summarizes the changes ported from Swift Tekkon v1.7.0 to TekkonCC.

## Breaking Changes

### Unicode Scalar Migration (char32_t)

The core data type for phonabets has been migrated from `std::string` to `char32_t` (Unicode scalar):

#### 1. Allowed Phonabet Constants

**Before (v1.6.0):**
```cpp
inline static std::vector<std::string> allowedConsonants = {
    "ㄅ", "ㄆ", "ㄇ", ...
};
```

**After (v1.7.0):**
```cpp
inline static std::vector<char32_t> allowedConsonants = {
    U'ㄅ', U'ㄆ', U'ㄇ', ...
};
```

This change applies to:
- `allowedConsonants`
- `allowedSemivowels`
- `allowedVowels`
- `allowedIntonations`
- `allowedPhonabets`

#### 2. Phonabet Structure

**Before (v1.6.0):**
- Stored value as `std::string strStorage`
- Constructor: `Phonabet(std::string input = "")`
- Access: `value()` returned internal string

**After (v1.7.0):**
- Stores value as `char32_t scalarValue`
- New constructor: `Phonabet(char32_t input)`
- Access: 
  - `value()` returns UTF-8 `std::string` (for compatibility)
  - `scalar()` returns `char32_t` (for direct access)
- Old string constructor still works for backward compatibility

#### 3. New Methods

Added `receiveKeyFromPhonabet(char32_t phonabet)` for efficient Unicode scalar handling.

The string-based version `receiveKeyFromPhonabet(std::string phonabet)` is maintained for backward compatibility and internally converts to char32_t.

## API Design

### Read-Only Properties

Following Swift v1.7.0's pattern of `public internal(set)` properties, the following Composer members are now documented as read-only (though still public in C++):

- `consonant`, `semivowel`, `vowel`, `intonation`
- `romajiBuffer`
- `parser`

**Recommended Usage:**
- ✅ Read these properties directly
- ❌ Avoid modifying them directly
- ✅ Use `receiveKey()`, `receiveSequence()`, or `clear()` to modify state
- ✅ Use `ensureParser()` to change the parser

## Helper Functions

Added `char32ToString(char32_t scalar)` to convert Unicode scalars to UTF-8 strings.

## Compatibility

### Backward Compatibility

- All existing string-based APIs remain functional
- Tests do not require changes
- External code using string-based APIs continues to work

### Performance Benefits

- Reduced string comparisons and conversions
- Single Unicode code point comparisons instead of multi-byte string operations
- More efficient phonabet validation

## Migration Guide

### For Library Users

If you're using TekkonCC through its public API:
- **No changes required** - backward compatible string APIs are maintained
- Consider using the new char32_t APIs for better performance when working with individual phonabets

### For Contributors

When working with phonabet internals:
- Use `char32_t` (e.g., `U'ㄅ'`) instead of `std::string` (e.g., `"ㄅ"`)
- Use `.scalar()` to get char32_t values from Phonabet
- Use `.value()` to get UTF-8 string representation

## Testing

All unit tests pass (23 tests total):
- 4 basic tests
- 6 intermediate tests (pinyin handling)
- 6 advanced tests (keyboard arrangements)
- 2 arrangement tests (using centralized data)
- 5 pinyin/trie tests (new PinyinTrie functionality)

100% test pass rate maintained throughout migration.

## Additional Features Ported (v1.7.0 Complete)

### PinyinTrie and Chopper

Implemented the complete PinyinTrie class for intelligent pinyin processing:

**Features:**
- `PinyinTrie` class with trie-based structure for efficient pinyin-to-zhuyin mapping
- `chop()` method: Intelligently segments continuous simplified pinyin strings
  - Example: "shjdaz" → ["sh", "j", "da", "z"]
  - Useful for smart pinyin input like Sogou Pinyin
- `deductChoppedPinyinToZhuyin()`: Converts chopped pinyin segments to zhuyin candidates
  - Example: ["b", "yue", "z"] → ["ㄅ", "ㄩㄝ", "ㄓ&ㄗ"]
- `insert()`, `search()`: Core trie operations for pinyin lookup
- Support for all pinyin parsers (Hanyu, Secondary, Yale, Hualuo, Universal, Wade-Giles)

**Use Cases:**
- Smart/fuzzy pinyin input
- Abbreviated pinyin support
- Pinyin auto-segmentation

### Test Data Consolidation

**Centralized Test Data:**
- Created `Tests/TestAssets_Tekkon/TekkonTestData.hh`
- Ported 1486 lines of test data from Swift v1.7.0
- Uses C++ raw string literals for maintainability
- Includes `testTable4DynamicLayouts` with comprehensive keyboard layout test cases

**Test Structure Reorganization:**
- `GTests/TekkonTests_Arrangements.cc`: Tests keyboard arrangements using centralized data
- `GTests/TekkonTests_Pinyin.cc`: Tests PinyinTrie and Chopper functionality
- Dynamic test generation from centralized data (matching Swift v1.7.0 approach)
- Modular test organization for better maintainability
