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

#ifndef ZKBENCH_GOOGLE_BENCHMARK_REPORTER_H_
#define ZKBENCH_GOOGLE_BENCHMARK_REPORTER_H_

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "benchmark/benchmark.h"

#include "zkbench/schema.h"

namespace zkbench {

/// Google Benchmark reporter that outputs zkbench-compatible JSON.
///
/// This reporter collects benchmark results and outputs them in the zkbench
/// JSON format with support for:
/// - Latency with confidence intervals (requires --benchmark_repetitions=N)
/// - Memory usage (from run.max_heapbytes_used)
/// - Throughput (from run.counters["items_per_second"])
/// - Test vectors (via BenchmarkContext)
/// - Arbitrary metadata (via BenchmarkContext)
///
/// Usage:
///   GoogleBenchmarkReporter reporter("my-impl", "1.0.0");
///   ::benchmark::RunSpecifiedBenchmarks(&reporter);
///   std::cout << reporter.ToJson() << std::endl;
class GoogleBenchmarkReporter : public ::benchmark::BenchmarkReporter {
 public:
  /// Creates a reporter with the given implementation name and version.
  ///
  /// @param implementation Name of the benchmark implementation.
  /// @param version Version string of the implementation.
  GoogleBenchmarkReporter(std::string_view implementation,
                          std::string_view version);

  /// Reports benchmark context (called once before runs).
  bool ReportContext(const Context& context) override;

  /// Reports individual benchmark runs.
  ///
  /// This method collects timing data from each run. When using
  /// --benchmark_repetitions=N, multiple runs are collected for calculating
  /// confidence intervals in Finalize().
  void ReportRuns(const std::vector<Run>& runs) override;

  /// Finalizes the report after all benchmarks complete.
  ///
  /// Calculates statistics (mean, stdev) and confidence intervals for
  /// benchmarks with multiple runs, and aggregates results into the final
  /// report.
  void Finalize() override;

  /// Returns the benchmark report.
  const BenchmarkReport& GetReport() const { return report_; }

  /// Serializes the report to JSON string.
  std::string ToJson() const;

 private:
  /// Extracts the clean benchmark name (removes suffixes like /iterations:X).
  static std::string ExtractBenchmarkName(const std::string& run_name);

  /// Converts Google Benchmark time unit to string.
  static std::string TimeUnitToString(::benchmark::TimeUnit unit);

  BenchmarkReport report_;

  /// Collected run times per benchmark for confidence interval calculation.
  std::map<std::string, std::vector<double>> run_times_ns_;

  /// Last run data per benchmark for extracting memory/throughput/iterations.
  struct RunData {
    double memory_bytes = 0;
    double items_per_second = 0;
    int64_t iterations = 0;
    ::benchmark::TimeUnit time_unit = ::benchmark::kNanosecond;
  };
  std::map<std::string, RunData> last_run_data_;
};

}  // namespace zkbench

#endif  // ZKBENCH_GOOGLE_BENCHMARK_REPORTER_H_
