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

#ifndef ZKBENCH_BENCHMARK_MAIN_H_
#define ZKBENCH_BENCHMARK_MAIN_H_

#include <string_view>

namespace zkbench {

/// Runs benchmarks with optional zkbench JSON output.
///
/// This function initializes Google Benchmark, parses zkbench-specific flags,
/// and runs all registered benchmarks. If --zkbench_out=<file> is specified,
/// it also runs benchmarks with the zkbench reporter and writes JSON output.
///
/// Usage in a cc_binary:
///   int main(int argc, char** argv) {
///     return zkbench::BenchmarkMain(argc, argv, "my-impl", "1.0.0");
///   }
///
/// Command line flags:
///   --zkbench_out=<file>  Write zkbench JSON output to the specified file.
///   All other flags are passed to Google Benchmark.
///
/// @param argc Argument count from main().
/// @param argv Argument values from main().
/// @param implementation Name of the benchmark implementation.
/// @param version Version string of the implementation.
/// @return Exit code (0 for success, 1 for error).
int BenchmarkMain(int argc, char** argv, std::string_view implementation,
                  std::string_view version);

}  // namespace zkbench

#endif  // ZKBENCH_BENCHMARK_MAIN_H_
