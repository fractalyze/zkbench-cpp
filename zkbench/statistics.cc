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
#include <numeric>
#include <stdexcept>

namespace zkbench {

std::pair<double, double> CalculateStatistics(
    const std::vector<double>& values) {
  if (values.empty()) {
    throw std::invalid_argument("Cannot calculate statistics on empty vector");
  }

  size_t n = values.size();
  double mean = std::accumulate(values.begin(), values.end(), 0.0) /
                static_cast<double>(n);

  if (n < 2) {
    return {mean, 0.0};
  }

  double variance = 0.0;
  for (double x : values) {
    double diff = x - mean;
    variance += diff * diff;
  }
  variance /= static_cast<double>(n - 1);
  double stdev = std::sqrt(variance);

  return {mean, stdev};
}

std::pair<double, double> CalculateConfidenceInterval(double mean, double stdev,
                                                      double confidence) {
  double z;
  constexpr double kEpsilon = 0.001;

  if (std::abs(confidence - 0.95) < kEpsilon) {
    z = 2.0;
  } else if (std::abs(confidence - 0.99) < kEpsilon) {
    z = 2.576;
  } else {
    z = 2.0;  // Default to 95% confidence
  }

  double margin = z * stdev;
  return {mean - margin, mean + margin};
}

}  // namespace zkbench
