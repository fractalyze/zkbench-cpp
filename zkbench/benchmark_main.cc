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

#include "zkbench/benchmark_main.h"

#include <cstring>
#include <fstream>
#include <iostream>

#include "benchmark/benchmark.h"

#include "zkbench/google_benchmark_reporter.h"

namespace zkbench {
namespace {

// Parses --zkbench_out=<file> flag and removes it from argv.
// Returns the output file path, or nullptr if not specified.
const char* ParseZkBenchFlag(int* argc, char** argv) {
  const char* output_file = nullptr;

  for (int i = 1; i < *argc; ++i) {
    if (std::strncmp(argv[i], "--zkbench_out=", 14) == 0) {
      output_file = argv[i] + 14;
      // Remove this argument from argv
      for (int j = i; j < *argc - 1; ++j) {
        argv[j] = argv[j + 1];
      }
      --(*argc);
      --i;  // Recheck this index since we shifted arguments
    }
  }

  return output_file;
}

}  // namespace

int BenchmarkMain(int argc, char** argv, std::string_view implementation,
                  std::string_view version) {
  // Parse zkbench-specific flags before Google Benchmark
  const char* zkbench_output_file = ParseZkBenchFlag(&argc, argv);

  // Initialize Google Benchmark
  ::benchmark::Initialize(&argc, argv);

  if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }

  // Run with console reporter (default)
  ::benchmark::RunSpecifiedBenchmarks();

  // If zkbench output is requested, run again with our reporter
  if (zkbench_output_file != nullptr) {
    GoogleBenchmarkReporter reporter(implementation, version);
    ::benchmark::RunSpecifiedBenchmarks(&reporter);
    reporter.Finalize();

    std::ofstream ofs(zkbench_output_file);
    if (ofs.is_open()) {
      ofs << reporter.ToJson() << std::endl;
      std::cerr << "zkbench JSON written to: " << zkbench_output_file
                << std::endl;
    } else {
      std::cerr << "Error: Could not open zkbench output file: "
                << zkbench_output_file << std::endl;
      return 1;
    }
  }

  ::benchmark::Shutdown();
  return 0;
}

}  // namespace zkbench
