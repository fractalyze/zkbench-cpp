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

#include "zkbench/utils.h"

#include "gtest/gtest.h"

namespace zkbench {
namespace {

TEST(UtilsTest, GetGitCommitSha) {
  std::string sha = GetGitCommitSha();

  // Should return either a 12-char SHA or "unknown"
  if (sha != "unknown") {
    EXPECT_EQ(sha.length(), 12);
    // Should be hexadecimal characters
    for (char c : sha) {
      EXPECT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
    }
  }
}

TEST(UtilsTest, GetTimestamp) {
  std::string timestamp = GetTimestamp();

  // Should be in ISO8601 format: YYYY-MM-DDTHH:MM:SS.ffffff+00:00
  // Example: 2026-01-31T12:48:27.436511+00:00
  EXPECT_EQ(timestamp.length(), 32);
  EXPECT_EQ(timestamp[4], '-');
  EXPECT_EQ(timestamp[7], '-');
  EXPECT_EQ(timestamp[10], 'T');
  EXPECT_EQ(timestamp[13], ':');
  EXPECT_EQ(timestamp[16], ':');
  EXPECT_EQ(timestamp[19], '.');
  EXPECT_EQ(timestamp.substr(26), "+00:00");
}

TEST(UtilsTest, GetTimestampYearIs2026OrLater) {
  std::string timestamp = GetTimestamp();

  // Extract year
  int year = std::stoi(timestamp.substr(0, 4));
  EXPECT_GE(year, 2026);
}

}  // namespace
}  // namespace zkbench
