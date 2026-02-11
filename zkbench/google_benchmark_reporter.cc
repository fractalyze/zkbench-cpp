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

#include "zkbench/google_benchmark_reporter.h"

#include "zkbench/benchmark_context.h"
#include "zkbench/statistics.h"

namespace zkbench {

GoogleBenchmarkReporter::GoogleBenchmarkReporter(
    std::string_view implementation, std::string_view version) {
  report_.metadata = Metadata::Create(implementation, version);
}

bool GoogleBenchmarkReporter::ReportContext(const Context& /*context*/) {
  // Context is already captured in metadata during construction.
  return true;
}

void GoogleBenchmarkReporter::ReportRuns(const std::vector<Run>& runs) {
  for (const auto& run : runs) {
    // Skip skipped benchmarks
    if (run.skipped != ::benchmark::internal::NotSkipped) {
      continue;
    }

    // Skip aggregated runs (mean, median, stddev, cv) - we calculate our own
    if (!run.aggregate_name.empty()) {
      continue;
    }

    std::string bench_name = ExtractBenchmarkName(run.run_name.str());

    // Collect timing data (always in nanoseconds for consistency)
    double time_ns = run.GetAdjustedRealTime();

    // Convert to nanoseconds based on time_unit
    switch (run.time_unit) {
      case ::benchmark::kNanosecond:
        // Already in nanoseconds
        break;
      case ::benchmark::kMicrosecond:
        time_ns *= 1e3;
        break;
      case ::benchmark::kMillisecond:
        time_ns *= 1e6;
        break;
      case ::benchmark::kSecond:
        time_ns *= 1e9;
        break;
    }

    run_times_ns_[bench_name].push_back(time_ns);

    // Store last run data for memory/throughput extraction
    RunData& data = last_run_data_[bench_name];
    data.iterations = run.iterations;
    data.time_unit = run.time_unit;

    // Extract memory if available
    if (run.memory_result.max_bytes_used > 0) {
      data.memory_bytes = static_cast<double>(run.memory_result.max_bytes_used);
    }

    // Extract throughput from counters
    auto it = run.counters.find("items_per_second");
    if (it != run.counters.end()) {
      data.items_per_second = it->second.value;
    }
  }
}

void GoogleBenchmarkReporter::Finalize() {
  for (const auto& [name, times] : run_times_ns_) {
    BenchmarkResult result;

    // Calculate latency with optional confidence interval
    if (times.size() == 1) {
      result.latency = MetricValue::Create(times[0], "ns");
    } else {
      auto [mean, stdev] = CalculateStatistics(times);
      auto [lower, upper] = CalculateConfidenceInterval(mean, stdev);
      result.latency = MetricValue::WithBounds(mean, "ns", lower, upper);
    }

    // Add iterations from last run
    auto run_it = last_run_data_.find(name);
    if (run_it != last_run_data_.end()) {
      const RunData& data = run_it->second;
      result.iterations = static_cast<size_t>(data.iterations);

      // Add memory if available
      if (data.memory_bytes > 0) {
        result.memory = MetricValue::Create(data.memory_bytes, "bytes");
      }

      // Add throughput if available
      if (data.items_per_second > 0) {
        result.throughput = MetricValue::Create(data.items_per_second, "ops/s");
      }
    }

    // Get test vectors and metadata from BenchmarkContext
    result.test_vectors = BenchmarkContext::GetTestVectors(name);
    result.metadata = BenchmarkContext::GetMetadata(name);

    report_.benchmarks[name] = result;
  }
}

std::string GoogleBenchmarkReporter::ToJson() const { return report_.ToJson(); }

std::string GoogleBenchmarkReporter::ExtractBenchmarkName(
    const std::string& run_name) {
  // Remove suffixes like "/iterations:X", "/repeats:N", etc.
  auto slash_pos = run_name.find('/');
  if (slash_pos != std::string::npos) {
    return run_name.substr(0, slash_pos);
  }
  return run_name;
}

std::string GoogleBenchmarkReporter::TimeUnitToString(
    ::benchmark::TimeUnit unit) {
  switch (unit) {
    case ::benchmark::kNanosecond:
      return "ns";
    case ::benchmark::kMicrosecond:
      return "us";
    case ::benchmark::kMillisecond:
      return "ms";
    case ::benchmark::kSecond:
      return "s";
  }
  return "ns";
}

}  // namespace zkbench
