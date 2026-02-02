/* Copyright 2026 The zkbench-cpp Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "zkbench/platform.h"

#include "gtest/gtest.h"

namespace zkbench {
namespace {

TEST(PlatformTest, GetOsName) {
  std::string os = GetOsName();
  EXPECT_FALSE(os.empty());
#if defined(__linux__)
  EXPECT_EQ(os, "linux");
#elif defined(__APPLE__)
  EXPECT_EQ(os, "macos");
#elif defined(_WIN32)
  EXPECT_EQ(os, "windows");
#endif
}

TEST(PlatformTest, GetArchName) {
  std::string arch = GetArchName();
  EXPECT_FALSE(arch.empty());
#if defined(__x86_64__) || defined(_M_X64)
  EXPECT_EQ(arch, "x86_64");
#elif defined(__aarch64__) || defined(_M_ARM64)
  EXPECT_EQ(arch, "aarch64");
#endif
}

TEST(PlatformTest, GetCpuCount) {
  int count = GetCpuCount();
  EXPECT_GE(count, 1);
}

TEST(PlatformTest, GetCpuVendor) {
  // This test just ensures the function doesn't crash.
  // The result depends on the platform.
  auto vendor = GetCpuVendor();
  // On most systems, we should get a vendor string
#if defined(__linux__) || defined(__APPLE__)
  // These platforms usually have CPU info available
  // But we don't fail if it's not available
  if (vendor.has_value()) {
    EXPECT_FALSE(vendor->empty());
  }
#endif
}

}  // namespace
}  // namespace zkbench
