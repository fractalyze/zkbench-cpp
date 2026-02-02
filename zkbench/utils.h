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

#ifndef ZKBENCH_UTILS_H_
#define ZKBENCH_UTILS_H_

#include <string>

namespace zkbench {

/// Gets the current git commit SHA (first 12 characters).
///
/// @return Truncated git commit SHA, or "unknown" if not in a git repository.
std::string GetGitCommitSha();

/// Gets the current timestamp in ISO8601 format.
///
/// @return Timestamp string in format "YYYY-MM-DDTHH:MM:SSZ".
std::string GetTimestamp();

}  // namespace zkbench

#endif  // ZKBENCH_UTILS_H_
