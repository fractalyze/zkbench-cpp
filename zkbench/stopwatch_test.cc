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

#include "zkbench/stopwatch.h"

#include <chrono>  // NOLINT(build/c++11)
#include <thread>  // NOLINT(build/c++11)

#include "gtest/gtest.h"

namespace zkbench {
namespace {

TEST(StopwatchTest, InitialState) {
  Stopwatch sw;
  EXPECT_FALSE(sw.IsRunning());
  EXPECT_EQ(sw.ElapsedNanos(), 0);
}

TEST(StopwatchTest, StartStop) {
  Stopwatch sw;
  sw.Start();
  EXPECT_TRUE(sw.IsRunning());

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  EXPECT_FALSE(sw.IsRunning());
  EXPECT_GT(sw.ElapsedNanos(), 0);
}

TEST(StopwatchTest, ElapsedMillis) {
  Stopwatch sw;
  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  // Should be at least 10ms (allowing some tolerance)
  EXPECT_GE(sw.ElapsedMillis(), 9.0);
}

TEST(StopwatchTest, ElapsedSeconds) {
  Stopwatch sw;
  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  sw.Stop();

  // Should be at least 0.1s (allowing some tolerance)
  EXPECT_GE(sw.ElapsedSeconds(), 0.09);
}

TEST(StopwatchTest, PauseResume) {
  Stopwatch sw;
  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Pause();

  int64_t paused_nanos = sw.ElapsedNanos();

  // Time shouldn't advance while paused
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_EQ(sw.ElapsedNanos(), paused_nanos);

  sw.Resume();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  // Should have accumulated more time after resume
  EXPECT_GT(sw.ElapsedNanos(), paused_nanos);
}

TEST(StopwatchTest, Reset) {
  Stopwatch sw;
  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  EXPECT_GT(sw.ElapsedNanos(), 0);

  sw.Reset();
  EXPECT_FALSE(sw.IsRunning());
  EXPECT_EQ(sw.ElapsedNanos(), 0);
}

TEST(StopwatchTest, MultipleStartStopCycles) {
  Stopwatch sw;

  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  int64_t first_cycle = sw.ElapsedNanos();

  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  // Time should accumulate across cycles
  EXPECT_GT(sw.ElapsedNanos(), first_cycle);
}

TEST(StopwatchTest, StartWhileRunningNoOp) {
  Stopwatch sw;
  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Second start should be a no-op
  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  // Should have approximately 20ms, not restarted at 10ms
  EXPECT_GE(sw.ElapsedMillis(), 18.0);
}

TEST(StopwatchTest, StopWhileStoppedNoOp) {
  Stopwatch sw;
  sw.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.Stop();

  int64_t elapsed = sw.ElapsedNanos();

  // Second stop should be a no-op
  sw.Stop();
  EXPECT_EQ(sw.ElapsedNanos(), elapsed);
}

}  // namespace
}  // namespace zkbench
