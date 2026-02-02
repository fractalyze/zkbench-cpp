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

#ifndef ZKBENCH_SCHEMA_H_
#define ZKBENCH_SCHEMA_H_

#include <map>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace zkbench {

/// Represents a benchmark metric with optional confidence bounds.
struct MetricValue {
  double value;
  std::string unit;
  std::optional<double> lower_value;
  std::optional<double> upper_value;

  /// Creates a MetricValue with just value and unit.
  static MetricValue Create(double value, std::string_view unit);

  /// Creates a MetricValue with confidence bounds.
  static MetricValue WithBounds(double value, std::string_view unit,
                                double lower, double upper);
};

/// Test vector verification information.
struct TestVectors {
  std::string input_hash;
  std::string output_hash;
  bool verified;
};

/// Represents results from a single benchmark.
struct BenchmarkResult {
  std::optional<MetricValue> latency;
  std::optional<MetricValue> memory;
  std::optional<MetricValue> throughput;
  size_t iterations = 0;
  std::optional<TestVectors> test_vectors;
  nlohmann::json metadata;
};

/// Platform information.
struct Platform {
  std::string os;
  std::string arch;
  int cpu_count;
  std::optional<std::string> cpu_vendor;

  /// Creates Platform with auto-detected values.
  static Platform Current();
};

/// Benchmark metadata.
struct Metadata {
  std::string implementation;
  std::string version;
  std::string commit_sha;  // 12 characters
  std::string timestamp;   // ISO8601
  Platform platform;

  /// Creates metadata with auto-detected platform and git info.
  static Metadata Create(std::string_view implementation,
                         std::string_view version);
};

/// Complete benchmark report.
struct BenchmarkReport {
  Metadata metadata;
  std::map<std::string, BenchmarkResult> benchmarks;

  /// Serializes report to JSON string.
  std::string ToJson(int indent = 2) const;

  /// Deserializes report from JSON string.
  static BenchmarkReport FromJson(std::string_view json);
};

// nlohmann/json serialization support
void to_json(nlohmann::json& j, const MetricValue& m);
void to_json(nlohmann::json& j, const TestVectors& tv);
void to_json(nlohmann::json& j, const BenchmarkResult& br);
void to_json(nlohmann::json& j, const Platform& p);
void to_json(nlohmann::json& j, const Metadata& m);
void to_json(nlohmann::json& j, const BenchmarkReport& r);

void from_json(const nlohmann::json& j, MetricValue& m);
void from_json(const nlohmann::json& j, TestVectors& tv);
void from_json(const nlohmann::json& j, BenchmarkResult& br);
void from_json(const nlohmann::json& j, Platform& p);
void from_json(const nlohmann::json& j, Metadata& m);
void from_json(const nlohmann::json& j, BenchmarkReport& r);

}  // namespace zkbench

#endif  // ZKBENCH_SCHEMA_H_
