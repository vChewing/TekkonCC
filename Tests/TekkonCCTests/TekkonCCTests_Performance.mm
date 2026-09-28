// (c) 2022 and onwards The vChewing Project (LGPL v3.0 License or later).
// ====================
// This code is released under the SPDX-License-Identifier: `LGPL-3.0-or-later`.

// ADVICE: Save as UTF8 without BOM signature!!!

// 效能基準測試。與 GTests/TekkonTests_Performance.cc 內容雷同，
// 僅測試框架不同。

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#import "Tekkon.hh"

using namespace Tekkon;

namespace {

/// 量測 `body` 的實際耗時（秒）。時間來源為 `std::chrono::steady_clock`。
template <typename Body>
double secondsOf(Body body) {
  const auto start = std::chrono::steady_clock::now();
  body();
  const auto end = std::chrono::steady_clock::now();
  return std::chrono::duration<double>(end - start).count();
}

/// 以 `digits` 位小數格式化秒數。
std::string formatSeconds(double seconds, int digits) {
  char buffer[64];
  std::snprintf(buffer, sizeof(buffer), "%.*f", digits, seconds);
  return std::string(buffer);
}

/// `MandarinParser` 本身沒有可讀的名稱標籤，故於測試端就地提供，
/// 僅供輸出之用。
std::string parserNameTag(MandarinParser parser) {
  switch (parser) {
    case ofDachen:
      return "Dachen";
    case ofDachen26:
      return "Dachen26";
    case ofETen:
      return "ETen";
    case ofETen26:
      return "ETen26";
    case ofHsu:
      return "Hsu";
    case ofIBM:
      return "IBM";
    case ofMiTAC:
      return "MiTAC";
    case ofSeigyou:
      return "Seigyou";
    case ofFakeSeigyou:
      return "FakeSeigyou";
    case ofStarlight:
      return "Starlight";
    case ofAlvinLiu:
      return "AlvinLiu";
    case ofHanyuPinyin:
      return "HanyuPinyin";
    case ofSecondaryPinyin:
      return "SecondaryPinyin";
    case ofYalePinyin:
      return "YalePinyin";
    case ofHualuoPinyin:
      return "HualuoPinyin";
    case ofUniversalPinyin:
      return "UniversalPinyin";
    case ofWadeGilesPinyin:
      return "WadeGilesPinyin";
  }
  return std::to_string(static_cast<int>(parser));
}

}  // namespace

@interface TekkonCCTests_Performance : XCTestCase

@end

@implementation TekkonCCTests_Performance

- (void)test_Performance_DynamicLayoutPerformance {
  const std::vector<std::string> testSequences = {
      "e", "r", "d", "y", "qu", "quu", "quur", "q", "qj", "qjo", "l", "lr"};
  const int iterations = 50;

  for (MandarinParser parser : allDynamicZhuyinCases()) {
    // 全程重用同一個組字器，不在迭代內重新建構。
    Composer composer("", parser);
    const double timeDelta = secondsOf([&]() {
      for (int i = 0; i < iterations; ++i) {
        for (const std::string& sequence : testSequences) {
          composer.clear();
          composer.receiveSequence(sequence);
        }
      }
    });

    const double avgTime = timeDelta / static_cast<double>(iterations);
    std::cout << " -> [Tekkon][(" << parserNameTag(parser) << ")] "
              << iterations << " iterations in " << formatSeconds(timeDelta, 4)
              << "s (avg: " << formatSeconds(avgTime, 6) << "s per iteration)"
              << std::endl;

    // 效能期望：每次迭代應該在 50ms 以內完成（含 12 個測試序列）；
    // Apple 平台放寬為 0.50s，以在不同平台環境中提供更好的可靠性。
#if defined(__APPLE__)
    const double avgTimeExpected = 0.50;
#else
    const double avgTimeExpected = 0.050;
#endif
    XCTAssertLessThan(avgTime, avgTimeExpected);
  }
}

- (void)test_Performance_StringProcessingPerformance {
  const std::vector<std::string> testStrings = {
      "ㄅㄆㄇㄈ", "ㄐㄑㄒ", "ㄓㄔㄕㄗㄘㄙ", "ㄧㄩ", "ㄛㄥ", "ㄟ"};
  const std::string targetChar = "ㄅ";
  const int iterations = 10000;

  const double processingTime = secondsOf([&]() {
    for (int i = 0; i < iterations; ++i) {
      for (const std::string& testString : testStrings) {
        stringInclusion(testString, targetChar);
      }
    }
  });

  std::cout << " -> [Tekkon] String processing (" << iterations
            << " iterations): " << formatSeconds(processingTime, 6) << "s"
            << std::endl;

  // 效能期望：字串處理應該相對較快。
  XCTAssertLessThan(processingTime, 0.2);
}

- (void)test_Performance_MemoryOptimization {
  const std::vector<std::string> testSequences = {"ba", "pa", "ma", "fa",
                                                  "da", "ta", "na", "la"};
  const int iterations = 50;

  // 測試物件重用：單一組字器服務全部迭代。
  Composer reusableComposer("", ofDachen26);
  const double reuseTime = secondsOf([&]() {
    for (int i = 0; i < iterations; ++i) {
      for (const std::string& sequence : testSequences) {
        reusableComposer.clear();
        reusableComposer.receiveSequence(sequence);
      }
    }
  });

  // 測試重新建立物件：每條序列各建一個組字器。
  const double recreateTime = secondsOf([&]() {
    for (int i = 0; i < iterations; ++i) {
      for (const std::string& sequence : testSequences) {
        Composer composer("", ofDachen26);
        composer.receiveSequence(sequence);
      }
    }
  });

  const double improvement = ((recreateTime - reuseTime) / recreateTime) * 100;

  std::cout << " -> [Tekkon] Object reuse: " << formatSeconds(reuseTime, 4)
            << "s vs recreation: " << formatSeconds(recreateTime, 4) << "s"
            << std::endl;
  std::cout << " -> [Tekkon] Memory optimization improvement: "
            << formatSeconds(improvement, 1) << "%" << std::endl;

  // 允許效能測量的變異性——現代最佳化之下，物件建立可能比重用更快。
  // 我們檢查重用效能沒有嚴重退化即可（允許 5x 的差異，因為測試環境和編譯器
  // 版本差異可能很大）。
  const double performanceTolerance = recreateTime * 5.0;
  XCTAssertLessThanOrEqual(reuseTime, performanceTolerance);
}

@end
