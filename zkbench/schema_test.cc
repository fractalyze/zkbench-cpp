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

#include "zkbench/schema.h"

#include "gtest/gtest.h"

namespace zkbench {
namespace {

TEST(MetricValueTest, Create) {
  auto metric = MetricValue::Create(100.5, "ns");
  EXPECT_DOUBLE_EQ(metric.value, 100.5);
  EXPECT_EQ(metric.unit, "ns");
  EXPECT_FALSE(metric.lower_value.has_value());
  EXPECT_FALSE(metric.upper_value.has_value());
}

TEST(MetricValueTest, WithBounds) {
  auto metric = MetricValue::WithBounds(100.0, "ms", 95.0, 105.0);
  EXPECT_DOUBLE_EQ(metric.value, 100.0);
  EXPECT_EQ(metric.unit, "ms");
  EXPECT_TRUE(metric.lower_value.has_value());
  EXPECT_TRUE(metric.upper_value.has_value());
  EXPECT_DOUBLE_EQ(metric.lower_value.value(), 95.0);
  EXPECT_DOUBLE_EQ(metric.upper_value.value(), 105.0);
}

TEST(MetricValueTest, JsonSerialization) {
  auto metric = MetricValue::Create(42.0, "ops/s");
  nlohmann::json j = metric;

  EXPECT_EQ(j["value"], 42.0);
  EXPECT_EQ(j["unit"], "ops/s");
  EXPECT_FALSE(j.contains("lower_value"));
  EXPECT_FALSE(j.contains("upper_value"));
}

TEST(MetricValueTest, JsonSerializationWithBounds) {
  auto metric = MetricValue::WithBounds(100.0, "ns", 90.0, 110.0);
  nlohmann::json j = metric;

  EXPECT_TRUE(j.contains("lower_value"));
  EXPECT_TRUE(j.contains("upper_value"));
  EXPECT_EQ(j["lower_value"], 90.0);
  EXPECT_EQ(j["upper_value"], 110.0);
}

TEST(MetricValueTest, JsonDeserialization) {
  nlohmann::json j = {{"value", 50.0}, {"unit", "MB"}};
  MetricValue metric = j.get<MetricValue>();

  EXPECT_DOUBLE_EQ(metric.value, 50.0);
  EXPECT_EQ(metric.unit, "MB");
  EXPECT_FALSE(metric.lower_value.has_value());
}

TEST(TestVectorsTest, JsonRoundtrip) {
  TestVectors tv{"abc123", "def456", true};
  nlohmann::json j = tv;
  TestVectors deserialized = j.get<TestVectors>();

  EXPECT_EQ(deserialized.input_hash, "abc123");
  EXPECT_EQ(deserialized.output_hash, "def456");
  EXPECT_TRUE(deserialized.verified);
}

TEST(BenchmarkResultTest, JsonSerializationSkipsNone) {
  BenchmarkResult result;
  result.latency = MetricValue::Create(100.0, "ns");

  nlohmann::json j = result;

  EXPECT_TRUE(j.contains("latency"));
  EXPECT_FALSE(j.contains("memory"));
  EXPECT_FALSE(j.contains("throughput"));
  EXPECT_FALSE(j.contains("iterations"));
  EXPECT_FALSE(j.contains("test_vectors"));
}

TEST(BenchmarkResultTest, JsonSerializationFull) {
  BenchmarkResult result;
  result.latency = MetricValue::Create(100.0, "ns");
  result.memory = MetricValue::Create(1024.0, "KB");
  result.throughput = MetricValue::Create(1000.0, "ops/s");
  result.iterations = 100;
  result.test_vectors = TestVectors{"input", "output", true};

  nlohmann::json j = result;
  BenchmarkResult deserialized = j.get<BenchmarkResult>();

  EXPECT_TRUE(deserialized.latency.has_value());
  EXPECT_TRUE(deserialized.memory.has_value());
  EXPECT_TRUE(deserialized.throughput.has_value());
  EXPECT_EQ(deserialized.iterations, 100);
  EXPECT_TRUE(deserialized.test_vectors.has_value());
}

TEST(PlatformTest, Current) {
  auto platform = Platform::Current();

  EXPECT_FALSE(platform.os.empty());
  EXPECT_FALSE(platform.arch.empty());
  EXPECT_GE(platform.cpu_count, 1);
}

TEST(PlatformTest, JsonSerialization) {
  auto platform = Platform::Current();
  nlohmann::json j = platform;

  EXPECT_TRUE(j.contains("os"));
  EXPECT_TRUE(j.contains("arch"));
  EXPECT_TRUE(j.contains("cpu_count"));
}

TEST(PlatformTest, JsonDeserialization) {
  nlohmann::json j = {{"os", "linux"}, {"arch", "x86_64"}, {"cpu_count", 8}};
  Platform platform = j.get<Platform>();

  EXPECT_EQ(platform.os, "linux");
  EXPECT_EQ(platform.arch, "x86_64");
  EXPECT_EQ(platform.cpu_count, 8);
  EXPECT_FALSE(platform.cpu_vendor.has_value());
}

TEST(PlatformTest, JsonSerializationSkipsCpuVendorWhenNone) {
  Platform platform{"linux", "x86_64", 4, std::nullopt};
  nlohmann::json j = platform;

  EXPECT_FALSE(j.contains("cpu_vendor"));
}

TEST(MetadataTest, Create) {
  auto metadata = Metadata::Create("test-impl", "1.0.0");

  EXPECT_EQ(metadata.implementation, "test-impl");
  EXPECT_EQ(metadata.version, "1.0.0");
  EXPECT_FALSE(metadata.commit_sha.empty());
  EXPECT_TRUE(metadata.timestamp.find('T') != std::string::npos);
}

TEST(BenchmarkReportTest, ToJson) {
  BenchmarkReport report;
  report.metadata = Metadata::Create("test", "0.1.0");
  report.benchmarks["bench1"] = BenchmarkResult{};
  report.benchmarks["bench1"].latency = MetricValue::Create(50.0, "ns");

  std::string json = report.ToJson();

  EXPECT_TRUE(json.find("metadata") != std::string::npos);
  EXPECT_TRUE(json.find("benchmarks") != std::string::npos);
  EXPECT_TRUE(json.find("bench1") != std::string::npos);
}

TEST(BenchmarkReportTest, FromJson) {
  std::string json = R"({
    "metadata": {
      "implementation": "test",
      "version": "1.0.0",
      "commit_sha": "abc123def456",
      "timestamp": "2026-02-03T12:00:00Z",
      "platform": {
        "os": "linux",
        "arch": "x86_64",
        "cpu_count": 8
      }
    },
    "benchmarks": {
      "my_bench": {
        "latency": {"value": 100.0, "unit": "ns"},
        "iterations": 1000
      }
    }
  })";

  BenchmarkReport report = BenchmarkReport::FromJson(json);

  EXPECT_EQ(report.metadata.implementation, "test");
  EXPECT_EQ(report.metadata.version, "1.0.0");
  EXPECT_GT(report.benchmarks.count("my_bench"), 0);
  EXPECT_TRUE(report.benchmarks["my_bench"].latency.has_value());
  EXPECT_DOUBLE_EQ(report.benchmarks["my_bench"].latency->value, 100.0);
}

TEST(BenchmarkReportTest, Roundtrip) {
  BenchmarkReport report;
  report.metadata = Metadata::Create("roundtrip-test", "2.0.0");
  report.benchmarks["my_bench"] = BenchmarkResult{};
  report.benchmarks["my_bench"].latency =
      MetricValue::WithBounds(100.0, "ns", 95.0, 105.0);
  report.benchmarks["my_bench"].throughput =
      MetricValue::Create(10000.0, "ops/s");
  report.benchmarks["my_bench"].iterations = 1000;

  std::string json = report.ToJson();
  BenchmarkReport deserialized = BenchmarkReport::FromJson(json);

  EXPECT_EQ(deserialized.metadata.implementation,
            report.metadata.implementation);
  EXPECT_EQ(deserialized.metadata.version, report.metadata.version);
  EXPECT_GT(deserialized.benchmarks.count("my_bench"), 0);
}

}  // namespace
}  // namespace zkbench
