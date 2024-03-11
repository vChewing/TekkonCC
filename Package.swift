// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "Tekkon",
  products: [
    // Products define the executables and libraries a package produces, making them visible to other packages.
    .library(
      name: "Tekkon",
      targets: ["Tekkon"]
    ),
    .executable(
      name: "TekkonCC_GTests",
      targets: ["TekkonCC_GTests"]
    )
  ],
  targets: [
    // Targets are the basic building blocks of a package, defining a module or a test suite.
    // Targets can depend on other targets in this package and products from dependencies.
    .target(
      name: "Tekkon",
      cxxSettings: [.unsafeFlags(["-fcxx-modules", "-fmodules"])]
    ),
    .testTarget(
      name: "TekkonCCTests",
      dependencies: ["Tekkon"]
      // swiftSettings: [.interoperabilityMode(.Cxx)]
    ),
    // MARK: - GoogleTest Targets
    .executableTarget(
      name: "TekkonCC_GTests",
      dependencies: ["gmocklib"],
      path: "GTests",
      cxxSettings: [
        .headerSearchPath("../Sources/Tekkon/include/"),
        .headerSearchPath("../utils/unittest/googlemock/include"),
        .headerSearchPath("../utils/unittest/googletest/include"),
      ]
    ),
    // MARK: - GoogleTest Dependency Targets
    .target(
      name: "gtestlib",
      path: "utils/unittest/googletest/src",
      exclude: [
        "gtest-death-test.cc",
        "gtest-filepath.cc",
        "gtest-matchers.cc",
        "gtest-port.cc",
        "gtest-printers.cc",
        "gtest-test-part.cc",
        "gtest-typed-test.cc",
        "gtest.cc",
      ],
      cxxSettings: [
        .headerSearchPath("../"),
        .headerSearchPath("../include"),
      ]
    ),
    .target(
      name: "gmocklib",
      dependencies: ["gtestlib"],
      path: "utils/unittest/googlemock/src",
      exclude: [
        "gmock-cardinalities.cc",
        "gmock-internal-utils.cc",
        "gmock-matchers.cc",
        "gmock-spec-builders.cc",
        "gmock.cc",
      ],
      cxxSettings: [
        .headerSearchPath("../"),
        .headerSearchPath("../include"),
        .headerSearchPath("../../googletest/include"),
      ]
    ),
  ],
  cxxLanguageStandard: .cxx17
)
