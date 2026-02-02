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

namespace zkbench {

void Stopwatch::Start() {
  if (running_) {
    return;
  }
  start_time_ = Clock::now();
  running_ = true;
  paused_ = false;
}

void Stopwatch::Stop() {
  if (!running_) {
    return;
  }
  if (!paused_) {
    auto end_time = Clock::now();
    accumulated_nanos_ += std::chrono::duration_cast<std::chrono::nanoseconds>(
                              end_time - start_time_)
                              .count();
  }
  running_ = false;
  paused_ = false;
}

void Stopwatch::Pause() {
  if (!running_ || paused_) {
    return;
  }
  auto pause_time = Clock::now();
  accumulated_nanos_ += std::chrono::duration_cast<std::chrono::nanoseconds>(
                            pause_time - start_time_)
                            .count();
  paused_ = true;
}

void Stopwatch::Resume() {
  if (!running_ || !paused_) {
    return;
  }
  start_time_ = Clock::now();
  paused_ = false;
}

void Stopwatch::Reset() {
  running_ = false;
  paused_ = false;
  accumulated_nanos_ = 0;
}

int64_t Stopwatch::ElapsedNanos() const {
  int64_t total = accumulated_nanos_;
  if (running_ && !paused_) {
    auto now = Clock::now();
    total +=
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time_)
            .count();
  }
  return total;
}

double Stopwatch::ElapsedMillis() const {
  return static_cast<double>(ElapsedNanos()) / 1e6;
}

double Stopwatch::ElapsedSeconds() const {
  return static_cast<double>(ElapsedNanos()) / 1e9;
}

bool Stopwatch::IsRunning() const { return running_; }

}  // namespace zkbench
