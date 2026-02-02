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

#ifndef ZKBENCH_BENCHMARK_CONTEXT_H_
#define ZKBENCH_BENCHMARK_CONTEXT_H_

#include <optional>
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "zkbench/schema.h"

namespace zkbench {

/// Thread-safe context for storing benchmark test vectors and metadata.
///
/// This class provides a global registry for benchmarks to store additional
/// information that will be included in the JSON output. Benchmarks can
/// register their test vectors (input/output hashes and verification status)
/// and arbitrary metadata before or during execution.
///
/// Usage:
///   // In benchmark setup or execution
///   BenchmarkContext::SetTestVectors("my_benchmark", input_hash, output_hash,
///                                    verified);
///   BenchmarkContext::SetMetadata("my_benchmark", {{"field", "BabyBear"}});
///
///   // The reporter will retrieve these when generating JSON output
class BenchmarkContext {
 public:
  /// Sets test vectors for a benchmark.
  ///
  /// @param benchmark_name The name of the benchmark.
  /// @param input_hash Hash of the benchmark input data.
  /// @param output_hash Hash of the benchmark output data.
  /// @param verified Whether the output was verified against expected values.
  static void SetTestVectors(std::string_view benchmark_name,
                             std::string_view input_hash,
                             std::string_view output_hash, bool verified);

  /// Sets metadata for a benchmark.
  ///
  /// @param benchmark_name The name of the benchmark.
  /// @param metadata JSON object containing benchmark-specific metadata.
  static void SetMetadata(std::string_view benchmark_name,
                          const nlohmann::json& metadata);

  /// Gets test vectors for a benchmark.
  ///
  /// @param benchmark_name The name of the benchmark.
  /// @return Test vectors if previously set, std::nullopt otherwise.
  static std::optional<TestVectors> GetTestVectors(
      std::string_view benchmark_name);

  /// Gets metadata for a benchmark.
  ///
  /// @param benchmark_name The name of the benchmark.
  /// @return Metadata JSON if previously set, empty object otherwise.
  static nlohmann::json GetMetadata(std::string_view benchmark_name);

  /// Clears all stored test vectors and metadata.
  ///
  /// Useful for testing or resetting state between benchmark runs.
  static void Clear();

  // Prevent instantiation
  BenchmarkContext() = delete;
};

}  // namespace zkbench

#endif  // ZKBENCH_BENCHMARK_CONTEXT_H_
