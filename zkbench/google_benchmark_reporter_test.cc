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

#include "gtest/gtest.h"

#include "zkbench/benchmark_context.h"

namespace zkbench {
namespace {

// Helper to create a Run with the given benchmark name.
// Note: real_accumulated_time is in seconds, time_unit is the display unit.
// GetAdjustedRealTime() returns (real_accumulated_time / iterations) converted
// to time_unit.
::benchmark::BenchmarkReporter::Run CreateRun(const std::string& name) {
  ::benchmark::BenchmarkReporter::Run run;
  run.run_name.function_name = name;
  run.skipped = ::benchmark::internal::NotSkipped;
  run.aggregate_name = "";
  run.iterations = 1000;
  run.time_unit = ::benchmark::kNanosecond;
  // real_accumulated_time is in SECONDS. For 100ns/iter with 1000 iters:
  // total = 100ns * 1000 = 100us = 100e-6 seconds = 0.0001 seconds
  run.real_accumulated_time = 100e-9 * 1000;  // 100ns * 1000 iterations
  return run;
}

class GoogleBenchmarkReporterTest : public ::testing::Test {
 protected:
  void SetUp() override { BenchmarkContext::Clear(); }
  void TearDown() override { BenchmarkContext::Clear(); }
};

TEST_F(GoogleBenchmarkReporterTest, ConstructorSetsMetadata) {
  GoogleBenchmarkReporter reporter("test-impl", "1.2.3");

  const auto& report = reporter.GetReport();
  EXPECT_EQ(report.metadata.implementation, "test-impl");
  EXPECT_EQ(report.metadata.version, "1.2.3");
  EXPECT_FALSE(report.metadata.commit_sha.empty());
  EXPECT_FALSE(report.metadata.timestamp.empty());
}

TEST_F(GoogleBenchmarkReporterTest, ReportContextReturnsTrue) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");
  ::benchmark::BenchmarkReporter::Context context;
  EXPECT_TRUE(reporter.ReportContext(context));
}

TEST_F(GoogleBenchmarkReporterTest, EmptyRunsProducesEmptyBenchmarks) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");
  reporter.ReportRuns({});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  EXPECT_TRUE(report.benchmarks.empty());
}

TEST_F(GoogleBenchmarkReporterTest, SingleRunProducesLatencyWithoutBounds) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  auto run = CreateRun("my_bench");

  reporter.ReportRuns({run});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  ASSERT_EQ(report.benchmarks.count("my_bench"), 1);

  const auto& result = report.benchmarks.at("my_bench");
  ASSERT_TRUE(result.latency.has_value());
  EXPECT_DOUBLE_EQ(result.latency->value, 100.0);
  EXPECT_EQ(result.latency->unit, "ns");
  EXPECT_FALSE(result.latency->lower_value.has_value());
  EXPECT_FALSE(result.latency->upper_value.has_value());
  EXPECT_EQ(result.iterations, 1000);
}

TEST_F(GoogleBenchmarkReporterTest, MultipleRunsProduceConfidenceInterval) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  // Create multiple runs (simulating --benchmark_repetitions=5)
  std::vector<::benchmark::BenchmarkReporter::Run> runs;
  for (int i = 0; i < 5; ++i) {
    auto run = CreateRun("my_bench");
    // Times: 90, 95, 100, 105, 110 ns per iteration
    // real_accumulated_time is in seconds
    run.real_accumulated_time =
        (90.0 + i * 5) * 1e-9 * 1000;  // (ns/iter) * (s/ns) * iters
    runs.push_back(run);
  }

  reporter.ReportRuns(runs);
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  ASSERT_EQ(report.benchmarks.count("my_bench"), 1);

  const auto& result = report.benchmarks.at("my_bench");
  ASSERT_TRUE(result.latency.has_value());
  EXPECT_NEAR(result.latency->value, 100.0, 0.001);  // Mean
  EXPECT_EQ(result.latency->unit, "ns");
  EXPECT_TRUE(result.latency->lower_value.has_value());
  EXPECT_TRUE(result.latency->upper_value.has_value());
  EXPECT_LT(result.latency->lower_value.value(), 100.0);
  EXPECT_GT(result.latency->upper_value.value(), 100.0);
}

TEST_F(GoogleBenchmarkReporterTest, SkippedRunsAreIgnored) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  auto run = CreateRun("skipped_bench");
  run.skipped = ::benchmark::internal::SkippedWithError;

  reporter.ReportRuns({run});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  EXPECT_TRUE(report.benchmarks.empty());
}

TEST_F(GoogleBenchmarkReporterTest, AggregatedRunsAreIgnored) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  // Individual run
  auto run1 = CreateRun("my_bench");

  // Aggregated run (mean) - should be ignored
  auto run2 = CreateRun("my_bench");
  run2.aggregate_name = "mean";

  reporter.ReportRuns({run1, run2});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  ASSERT_EQ(report.benchmarks.count("my_bench"), 1);
}

TEST_F(GoogleBenchmarkReporterTest, BenchmarkNameSuffixesAreStripped) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  auto run = CreateRun("my_bench/iterations:10000");
  run.iterations = 10000;
  run.real_accumulated_time = 100e-9 * 10000;  // 100ns * 10000 iters in seconds

  reporter.ReportRuns({run});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  EXPECT_EQ(report.benchmarks.count("my_bench"), 1);
  EXPECT_EQ(report.benchmarks.count("my_bench/iterations:10000"), 0);
}

TEST_F(GoogleBenchmarkReporterTest, TimeUnitConversion) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  // Run with milliseconds display unit
  auto run = CreateRun("ms_bench");
  run.time_unit = ::benchmark::kMillisecond;
  run.iterations = 100;
  // 1ms/iter = 0.001 seconds/iter, total = 0.001 * 100 = 0.1 seconds
  run.real_accumulated_time = 0.001 * 100;

  reporter.ReportRuns({run});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  const auto& result = report.benchmarks.at("ms_bench");
  ASSERT_TRUE(result.latency.has_value());
  // 1ms = 1,000,000ns
  EXPECT_NEAR(result.latency->value, 1e6, 0.001);
  EXPECT_EQ(result.latency->unit, "ns");
}

TEST_F(GoogleBenchmarkReporterTest, TestVectorsFromContext) {
  BenchmarkContext::SetTestVectors("my_bench", "input123", "output456", true);

  GoogleBenchmarkReporter reporter("test", "1.0.0");

  auto run = CreateRun("my_bench");

  reporter.ReportRuns({run});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  const auto& result = report.benchmarks.at("my_bench");
  ASSERT_TRUE(result.test_vectors.has_value());
  EXPECT_EQ(result.test_vectors->input_hash, "input123");
  EXPECT_EQ(result.test_vectors->output_hash, "output456");
  EXPECT_TRUE(result.test_vectors->verified);
}

TEST_F(GoogleBenchmarkReporterTest, MetadataFromContext) {
  nlohmann::json metadata = {{"field", "BabyBear"}, {"width", 16}};
  BenchmarkContext::SetMetadata("my_bench", metadata);

  GoogleBenchmarkReporter reporter("test", "1.0.0");

  auto run = CreateRun("my_bench");

  reporter.ReportRuns({run});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  const auto& result = report.benchmarks.at("my_bench");
  EXPECT_FALSE(result.metadata.empty());
  EXPECT_EQ(result.metadata["field"], "BabyBear");
  EXPECT_EQ(result.metadata["width"], 16);
}

TEST_F(GoogleBenchmarkReporterTest, ToJsonProducesValidJson) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  auto run = CreateRun("my_bench");

  reporter.ReportRuns({run});
  reporter.Finalize();

  std::string json = reporter.ToJson();
  EXPECT_FALSE(json.empty());
  EXPECT_NE(json.find("metadata"), std::string::npos);
  EXPECT_NE(json.find("benchmarks"), std::string::npos);
  EXPECT_NE(json.find("my_bench"), std::string::npos);

  // Verify it's valid JSON by parsing it
  nlohmann::json parsed = nlohmann::json::parse(json);
  EXPECT_TRUE(parsed.contains("metadata"));
  EXPECT_TRUE(parsed.contains("benchmarks"));
}

TEST_F(GoogleBenchmarkReporterTest, MultipleBenchmarks) {
  GoogleBenchmarkReporter reporter("test", "1.0.0");

  auto run1 = CreateRun("bench1");

  auto run2 = CreateRun("bench2");
  run2.iterations = 2000;
  run2.real_accumulated_time = 200e-9 * 2000;  // 200ns * 2000 iters

  reporter.ReportRuns({run1, run2});
  reporter.Finalize();

  const auto& report = reporter.GetReport();
  EXPECT_EQ(report.benchmarks.size(), 2);
  EXPECT_EQ(report.benchmarks.count("bench1"), 1);
  EXPECT_EQ(report.benchmarks.count("bench2"), 1);
}

}  // namespace
}  // namespace zkbench
