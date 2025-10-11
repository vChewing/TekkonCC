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

All 16 existing unit tests pass without modification:
- 4 basic tests
- 6 intermediate tests (pinyin handling)
- 6 advanced tests (keyboard arrangements)

## Future Work (Optional)

The following features from Swift v1.7.0 are not yet ported:
- Test data consolidation (centralized test data files)
- PinyinTrie and Chopper features for advanced pinyin processing
- Test file restructuring (TekkonTests_Arrangements, TekkonTests_Basic, TekkonTests_Pinyin)

These are optional enhancements and not required for v1.7.0 compatibility.
