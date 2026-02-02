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

#include "zkbench/benchmark_context.h"

#include <thread>  // NOLINT(build/c++11)
#include <vector>

#include "gtest/gtest.h"

namespace zkbench {
namespace {

class BenchmarkContextTest : public ::testing::Test {
 protected:
  void SetUp() override { BenchmarkContext::Clear(); }
  void TearDown() override { BenchmarkContext::Clear(); }
};

TEST_F(BenchmarkContextTest, SetAndGetTestVectors) {
  BenchmarkContext::SetTestVectors("my_bench", "input123", "output456", true);

  auto result = BenchmarkContext::GetTestVectors("my_bench");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->input_hash, "input123");
  EXPECT_EQ(result->output_hash, "output456");
  EXPECT_TRUE(result->verified);
}

TEST_F(BenchmarkContextTest, GetTestVectorsReturnsNulloptForUnknown) {
  auto result = BenchmarkContext::GetTestVectors("unknown_bench");
  EXPECT_FALSE(result.has_value());
}

TEST_F(BenchmarkContextTest, SetAndGetMetadata) {
  nlohmann::json metadata = {{"field", "BabyBear"}, {"width", 16}};
  BenchmarkContext::SetMetadata("my_bench", metadata);

  auto result = BenchmarkContext::GetMetadata("my_bench");
  EXPECT_EQ(result["field"], "BabyBear");
  EXPECT_EQ(result["width"], 16);
}

TEST_F(BenchmarkContextTest, GetMetadataReturnsEmptyObjectForUnknown) {
  auto result = BenchmarkContext::GetMetadata("unknown_bench");
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.empty());
}

TEST_F(BenchmarkContextTest, ClearRemovesAll) {
  BenchmarkContext::SetTestVectors("bench1", "in", "out", false);
  BenchmarkContext::SetMetadata("bench2", {{"key", "value"}});

  BenchmarkContext::Clear();

  EXPECT_FALSE(BenchmarkContext::GetTestVectors("bench1").has_value());
  EXPECT_TRUE(BenchmarkContext::GetMetadata("bench2").empty());
}

TEST_F(BenchmarkContextTest, OverwriteTestVectors) {
  BenchmarkContext::SetTestVectors("bench", "old_in", "old_out", false);
  BenchmarkContext::SetTestVectors("bench", "new_in", "new_out", true);

  auto result = BenchmarkContext::GetTestVectors("bench");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->input_hash, "new_in");
  EXPECT_EQ(result->output_hash, "new_out");
  EXPECT_TRUE(result->verified);
}

TEST_F(BenchmarkContextTest, OverwriteMetadata) {
  BenchmarkContext::SetMetadata("bench", {{"old_key", "old_value"}});
  BenchmarkContext::SetMetadata("bench", {{"new_key", "new_value"}});

  auto result = BenchmarkContext::GetMetadata("bench");
  EXPECT_FALSE(result.contains("old_key"));
  EXPECT_EQ(result["new_key"], "new_value");
}

TEST_F(BenchmarkContextTest, MultipleBenchmarks) {
  BenchmarkContext::SetTestVectors("bench1", "in1", "out1", true);
  BenchmarkContext::SetTestVectors("bench2", "in2", "out2", false);
  BenchmarkContext::SetMetadata("bench1", {{"key1", 1}});
  BenchmarkContext::SetMetadata("bench2", {{"key2", 2}});

  auto tv1 = BenchmarkContext::GetTestVectors("bench1");
  auto tv2 = BenchmarkContext::GetTestVectors("bench2");
  auto md1 = BenchmarkContext::GetMetadata("bench1");
  auto md2 = BenchmarkContext::GetMetadata("bench2");

  ASSERT_TRUE(tv1.has_value());
  ASSERT_TRUE(tv2.has_value());
  EXPECT_EQ(tv1->input_hash, "in1");
  EXPECT_EQ(tv2->input_hash, "in2");
  EXPECT_EQ(md1["key1"], 1);
  EXPECT_EQ(md2["key2"], 2);
}

TEST_F(BenchmarkContextTest, ThreadSafety) {
  constexpr int kNumThreads = 10;
  constexpr int kIterationsPerThread = 100;

  std::vector<std::thread> threads;
  threads.reserve(kNumThreads);

  for (int t = 0; t < kNumThreads; ++t) {
    threads.emplace_back([t]() {
      for (int i = 0; i < kIterationsPerThread; ++i) {
        std::string name =
            "bench_" + std::to_string(t) + "_" + std::to_string(i);
        BenchmarkContext::SetTestVectors(name, "in", "out", true);
        BenchmarkContext::SetMetadata(name, {{"iter", i}});
        BenchmarkContext::GetTestVectors(name);
        BenchmarkContext::GetMetadata(name);
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  // Verify some data was stored correctly
  auto tv = BenchmarkContext::GetTestVectors("bench_0_0");
  ASSERT_TRUE(tv.has_value());
  EXPECT_EQ(tv->input_hash, "in");
}

}  // namespace
}  // namespace zkbench
