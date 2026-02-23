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

#include "zkbench/statistics.h"

#include <cmath>
#include <stdexcept>

#include "gtest/gtest.h"

namespace zkbench {
namespace {

TEST(StatisticsTest, CalculateStatisticsMean) {
  std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
  auto [mean, stdev] = CalculateStatistics(values);

  EXPECT_DOUBLE_EQ(mean, 3.0);
}

TEST(StatisticsTest, CalculateStatisticsStdev) {
  std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
  auto [mean, stdev] = CalculateStatistics(values);

  // Sample stdev with n-1: sqrt(10/4) = sqrt(2.5) ≈ 1.5811
  EXPECT_NEAR(stdev, 1.5811, 0.001);
}

TEST(StatisticsTest, CalculateStatisticsSingleValue) {
  std::vector<double> values = {42.0};
  auto [mean, stdev] = CalculateStatistics(values);

  EXPECT_DOUBLE_EQ(mean, 42.0);
  EXPECT_DOUBLE_EQ(stdev, 0.0);
}

TEST(StatisticsTest, CalculateStatisticsThrowsOnEmpty) {
  std::vector<double> values;
  EXPECT_THROW(CalculateStatistics(values), std::invalid_argument);
}

TEST(StatisticsTest, CalculateConfidenceInterval95) {
  // mean=100, stdev=10, n=25 → se=2, margin=3.92
  auto [lower, upper] = CalculateConfidenceInterval(100.0, 10.0, 25, 0.95);

  EXPECT_NEAR(lower, 96.08, 0.0001);
  EXPECT_NEAR(upper, 103.92, 0.0001);
}

TEST(StatisticsTest, CalculateConfidenceInterval99) {
  // mean=100, stdev=10, n=25 → se=2, margin=5.152
  auto [lower, upper] = CalculateConfidenceInterval(100.0, 10.0, 25, 0.99);

  EXPECT_NEAR(lower, 100.0 - 5.152, 0.001);
  EXPECT_NEAR(upper, 100.0 + 5.152, 0.001);
}

TEST(StatisticsTest, CalculateConfidenceIntervalDefaultsTo95) {
  auto [lower1, upper1] = CalculateConfidenceInterval(100.0, 10.0, 25);
  auto [lower2, upper2] = CalculateConfidenceInterval(100.0, 10.0, 25, 0.95);

  EXPECT_DOUBLE_EQ(lower1, lower2);
  EXPECT_DOUBLE_EQ(upper1, upper2);
}

TEST(StatisticsTest, CalculateConfidenceIntervalSingleSample) {
  // n=1: se = stdev, margin = 1.96 × stdev
  auto [lower, upper] = CalculateConfidenceInterval(50.0, 5.0, 1, 0.95);

  EXPECT_NEAR(lower, 40.2, 0.0001);
  EXPECT_NEAR(upper, 59.8, 0.0001);
}

TEST(StatisticsTest, CalculateConfidenceIntervalZeroNThrows) {
  EXPECT_THROW(CalculateConfidenceInterval(100.0, 10.0, 0),
               std::invalid_argument);
}

}  // namespace
}  // namespace zkbench
