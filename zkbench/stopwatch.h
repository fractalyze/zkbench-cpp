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

#ifndef ZKBENCH_STOPWATCH_H_
#define ZKBENCH_STOPWATCH_H_

#include <chrono>  // NOLINT(build/c++11)
#include <cstdint>

namespace zkbench {

/// A simple stopwatch for timing benchmarks using std::chrono.
class Stopwatch {
 public:
  /// Starts the stopwatch. Has no effect if already running.
  void Start();

  /// Stops the stopwatch and accumulates elapsed time.
  void Stop();

  /// Pauses the stopwatch, preserving accumulated time.
  void Pause();

  /// Resumes a paused stopwatch.
  void Resume();

  /// Resets the stopwatch to zero and stops it.
  void Reset();

  /// Returns the total elapsed time in nanoseconds.
  int64_t ElapsedNanos() const;

  /// Returns the total elapsed time in milliseconds.
  double ElapsedMillis() const;

  /// Returns the total elapsed time in seconds.
  double ElapsedSeconds() const;

  /// Returns true if the stopwatch is currently running.
  bool IsRunning() const;

 private:
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  TimePoint start_time_;
  int64_t accumulated_nanos_ = 0;
  bool running_ = false;
  bool paused_ = false;
};

/// RAII guard that starts a `Stopwatch` on construction and stops it on
/// destruction. Useful for timing a scope that has multiple exit paths
/// (early returns, exceptions) without manually pairing every `Stop`
/// with its `Start`.
///
/// Example:
///   Stopwatch sw;
///   {
///     ScopedStopwatch guard(sw);
///     RunBenchmarkIteration();  // sw.Stop() fires even on throw/return
///   }
///   record_latency(sw.ElapsedMillis());
///
/// Nested guards on the same `Stopwatch` are safe: only the outermost
/// guard (the one that observed the stopwatch as not-running at
/// construction) issues `Start`/`Stop`. Inner guards are no-ops, so the
/// outer scope's timing remains correct.
class ScopedStopwatch {
 public:
  explicit ScopedStopwatch(Stopwatch& sw)
      : sw_(sw), owns_timer_(!sw.IsRunning()) {
    if (owns_timer_) sw_.Start();
  }
  ~ScopedStopwatch() {
    if (owns_timer_) sw_.Stop();
  }

  ScopedStopwatch(const ScopedStopwatch&) = delete;
  ScopedStopwatch& operator=(const ScopedStopwatch&) = delete;
  ScopedStopwatch(ScopedStopwatch&&) = delete;
  ScopedStopwatch& operator=(ScopedStopwatch&&) = delete;

 private:
  Stopwatch& sw_;
  bool owns_timer_;
};

}  // namespace zkbench

#endif  // ZKBENCH_STOPWATCH_H_
