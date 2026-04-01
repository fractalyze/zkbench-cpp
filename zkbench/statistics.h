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

#ifndef ZKBENCH_STATISTICS_H_
#define ZKBENCH_STATISTICS_H_

#include <utility>
#include <vector>

namespace zkbench {

/// Calculates mean and sample standard deviation (n - 1 denominator).
///
/// @param values Sequence of numeric values.
/// @return Pair of (mean, standard_deviation).
/// @throws std::invalid_argument if values is empty.
std::pair<double, double> CalculateStatistics(
    const std::vector<double>& values);

/// Calculates confidence interval bounds for the sample mean.
///
/// Uses the formula: mean ± z × (stdev / √n), where stdev / √n is the
/// standard error of the mean.
///
/// z-score approximation:
/// - 95% confidence: z = 1.96
/// - 99% confidence: z = 2.576
///
/// @param mean Sample mean.
/// @param stdev Sample standard deviation.
/// @param n Sample size.
/// @param confidence Confidence level (default 0.95 for 95%).
/// @return Pair of (lower_bound, upper_bound).
/// @throws std::invalid_argument if n is zero.
std::pair<double, double> CalculateConfidenceInterval(double mean, double stdev,
                                                      size_t n,
                                                      double confidence = 0.95);

}  // namespace zkbench

#endif  // ZKBENCH_STATISTICS_H_
