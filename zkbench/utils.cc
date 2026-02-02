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

#include "zkbench/utils.h"

#include <array>
#include <chrono>  // NOLINT(build/c++11)
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>

namespace zkbench {

std::string GetGitCommitSha() {
#if defined(_WIN32)
  std::unique_ptr<FILE, decltype(&_pclose)> pipe(
      _popen("git rev-parse HEAD", "r"), _pclose);
#else
  std::unique_ptr<FILE, decltype(&pclose)> pipe(
      popen("git rev-parse HEAD", "r"), pclose);
#endif

  if (!pipe) {
    return "unknown";
  }

  std::array<char, 128> buffer;
  std::string result;
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    result += buffer.data();
  }

  // Remove trailing newline
  while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
    result.pop_back();
  }

  // Return first 12 characters
  if (result.length() >= 12) {
    return result.substr(0, 12);
  }

  return result.empty() ? "unknown" : result;
}

std::string GetTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);

  // Get microseconds
  auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
                    now.time_since_epoch()) %
                1000000;

  std::tm tm_utc;
#if defined(_WIN32)
  gmtime_s(&tm_utc, &time_t_now);
#else
  gmtime_r(&time_t_now, &tm_utc);
#endif

  std::ostringstream oss;
  oss << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%S");
  oss << '.' << std::setfill('0') << std::setw(6) << micros.count();
  oss << "+00:00";
  return oss.str();
}

}  // namespace zkbench
