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

#ifndef ZKBENCH_PLATFORM_H_
#define ZKBENCH_PLATFORM_H_

#include <optional>
#include <string>

namespace zkbench {

/// Returns the OS name: "linux", "macos", "windows".
std::string GetOsName();

/// Returns the architecture name: "x86_64", "aarch64", etc.
std::string GetArchName();

/// Returns the number of CPU cores.
int GetCpuCount();

/// Detects CPU vendor/model string.
///
/// Returns CPU vendor information from:
/// - Linux: /proc/cpuinfo
/// - macOS: sysctl -n machdep.cpu.brand_string
/// - Windows: PROCESSOR_IDENTIFIER environment variable
std::optional<std::string> GetCpuVendor();

/// Detects GPU vendor/model string.
///
/// Returns GPU information from:
/// - macOS: system_profiler SPDisplaysDataType (Chipset Model)
/// - Linux: nvidia-smi or rocm-smi
/// For multi-GPU systems, returns only the first GPU.
std::optional<std::string> GetGpuVendor();

}  // namespace zkbench

#endif  // ZKBENCH_PLATFORM_H_
