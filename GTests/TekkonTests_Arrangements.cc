// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

#include <sstream>
#include <vector>

#include "../Sources/Tekkon/include/Tekkon.hh"
#include "../Tests/TestAssets_Tekkon/TekkonTestData.hh"
#include "gtest/gtest.h"

namespace Tekkon {

// Helper function to split string by delimiter
std::vector<std::string> splitString(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

// Helper function to replace underscores with spaces
std::string replaceUnderscores(const std::string& str) {
  std::string result = str;
  std::replace(result.begin(), result.end(), '_', ' ');
  return result;
}

// SubTestCase structure
struct SubTestCase {
  MandarinParser parser;
  std::string typing;
  std::string expected;

  SubTestCase(MandarinParser p, const std::string& t, const std::string& e)
      : parser(p),
        typing(replaceUnderscores(t)),
        expected(replaceUnderscores(e)) {}

  bool verify() {
    Composer composer("", parser);
    std::string strResult = composer.receiveSequence(typing);
    if (strResult == expected) return true;

    std::cout << "MISMATCH: \"" << typing << "\" -> \"" << strResult
              << "\" != \"" << expected << "\"" << std::endl;
    return false;
  }
};

// Test static keyboard arrangement (Dachen)
TEST(TekkonTests_Arrangements, QwertyDachenKeys) {
  Composer c("", ofDachen);
  int counter = 0;

  auto checkEq = [&](const std::string& keyStrokes,
                     const std::string& expected) {
    std::string result = c.receiveSequence(keyStrokes);
    if (result != expected) {
      std::cout << "MISMATCH (Dachen): \"" << keyStrokes << "\" -> \"" << result
                << "\" != \"" << expected << "\"" << std::endl;
      counter++;
    }
  };

  checkEq(" ", " ");
  checkEq("18 ", "ㄅㄚ ");
  checkEq("m,4", "ㄩㄝˋ");
  checkEq("5j/ ", "ㄓㄨㄥ ");
  checkEq("fu.", "ㄑㄧㄡ");
  checkEq("g0 ", "ㄕㄢ ");
  checkEq("xup6", "ㄌㄧㄣˊ");
  checkEq("xu;6", "ㄌㄧㄤˊ");
  checkEq("z/", "ㄈㄥ");
  checkEq("tjo ", "ㄔㄨㄟ ");
  checkEq("284", "ㄉㄚˋ");
  checkEq("2u4", "ㄉㄧˋ");
  checkEq("hl3", "ㄘㄠˇ");
  checkEq("5 ", "ㄓ ");
  checkEq("193", "ㄅㄞˇ");

  ASSERT_EQ(counter, 0);
}

// Test dynamic keyboard arrangements using centralized test data
TEST(TekkonTests_Arrangements, DynamicKeyLayouts) {
  std::string testData = TekkonTestData::testTable4DynamicLayouts;
  std::vector<std::string> lines = splitString(testData, '\n');

  // Dynamic parsers to test
  std::vector<MandarinParser> dynamicParsers = {ofDachen26, ofETen26, ofHsu,
                                                ofStarlight, ofAlvinLiu};

  for (size_t parserIdx = 0; parserIdx < dynamicParsers.size(); parserIdx++) {
    MandarinParser parser = dynamicParsers[parserIdx];
    std::vector<SubTestCase> testCases;

    bool isTitleLine = true;
    for (const auto& line : lines) {
      if (line.empty()) continue;

      if (isTitleLine) {
        isTitleLine = false;
        continue;
      }

      std::vector<std::string> cells = splitString(line, ' ');
      if (cells.size() < 6)
        continue;  // Need at least expected + 5 parser columns

      std::string expected = cells[0];
      // Column index: 0=expected, 1=Dachen26, 2=ETen26, 3=Hsu, 4=Starlight,
      // 5=AlvinLiu
      std::string typing = cells[parserIdx + 1];

      if (!typing.empty() && typing[0] != '`') {
        testCases.emplace_back(parser, typing, expected);
      }
    }

    std::cout << " -> [Tekkon] Testing parser " << static_cast<int>(parser)
              << "..." << std::endl;

    int failureCount = 0;
    for (auto& testCase : testCases) {
      if (!testCase.verify()) {
        failureCount++;
      }
    }

    ASSERT_EQ(failureCount, 0) << "Parser " << static_cast<int>(parser)
                               << " failed with " << failureCount << " errors";
  }
}

}  // namespace Tekkon
