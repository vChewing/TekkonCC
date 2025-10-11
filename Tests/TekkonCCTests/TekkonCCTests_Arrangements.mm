// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#import "../../TestAssets_Tekkon/TekkonTestData.hh"
#import "Tekkon.hh"

using namespace Tekkon;

@interface TekkonCCTests_Arrangements : XCTestCase

@end

@implementation TekkonCCTests_Arrangements

// Helper function to split string by delimiter
- (NSArray<NSString*>*)splitString:(NSString*)str byDelimiter:(char)delimiter {
  NSMutableArray<NSString*>* tokens = [NSMutableArray array];
  std::string cppStr = [str UTF8String];
  std::stringstream ss(cppStr);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    [tokens addObject:[NSString stringWithUTF8String:token.c_str()]];
  }
  return tokens;
}

// Helper function to replace underscores with spaces
- (NSString*)replaceUnderscores:(NSString*)str {
  return [str stringByReplacingOccurrencesOfString:@"_" withString:@" "];
}

// Test static keyboard arrangement (Dachen)
- (void)test_Arrangements_QwertyDachenKeys {
  Composer c("", ofDachen);
  int counter = 0;

  auto checkEq = [&](const std::string& keyStrokes,
                     const std::string& expected) {
    std::string result = c.receiveSequence(keyStrokes);
    if (result != expected) {
      NSLog(@"MISMATCH (Dachen): \"%s\" -> \"%s\" != \"%s\"",
            keyStrokes.c_str(), result.c_str(), expected.c_str());
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

  XCTAssertEqual(counter, 0);
}

// Test dynamic keyboard arrangements using centralized test data
- (void)test_Arrangements_DynamicKeyLayouts {
  std::string testDataCpp = TekkonTestData::testTable4DynamicLayouts;
  NSString* testData = [NSString stringWithUTF8String:testDataCpp.c_str()];
  NSArray<NSString*>* lines = [self splitString:testData byDelimiter:'\n'];

  // Dynamic parsers to test
  std::vector<MandarinParser> dynamicParsers = {ofDachen26, ofETen26, ofHsu,
                                                ofStarlight, ofAlvinLiu};

  for (size_t parserIdx = 0; parserIdx < dynamicParsers.size(); parserIdx++) {
    MandarinParser parser = dynamicParsers[parserIdx];
    int failureCount = 0;

    BOOL isTitleLine = YES;
    for (NSString* line in lines) {
      if ([line length] == 0) continue;

      if (isTitleLine) {
        isTitleLine = NO;
        continue;
      }

      NSArray<NSString*>* cells = [self splitString:line byDelimiter:' '];
      if ([cells count] < 6)
        continue;  // Need at least expected + 5 parser columns

      NSString* expectedNS = cells[0];
      // Column index: 0=expected, 1=Dachen26, 2=ETen26, 3=Hsu, 4=Starlight,
      // 5=AlvinLiu
      NSString* typingNS = cells[parserIdx + 1];

      if ([typingNS length] > 0 && [typingNS characterAtIndex:0] != '`') {
        std::string typing = [[self replaceUnderscores:typingNS] UTF8String];
        std::string expected =
            [[self replaceUnderscores:expectedNS] UTF8String];

        Composer composer("", parser);
        std::string strResult = composer.receiveSequence(typing);
        if (strResult != expected) {
          NSLog(@"MISMATCH: \"%s\" -> \"%s\" != \"%s\"", typing.c_str(),
                strResult.c_str(), expected.c_str());
          failureCount++;
        }
      }
    }

    NSLog(@" -> [Tekkon] Testing parser %d...", static_cast<int>(parser));
    XCTAssertEqual(failureCount, 0, @"Parser %d failed with %d errors",
                   static_cast<int>(parser), failureCount);
  }
}

@end
