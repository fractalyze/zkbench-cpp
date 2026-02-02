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

#include "zkbench/platform.h"

#include <cstdlib>
#include <fstream>
#include <regex>   // NOLINT(build/c++11)
#include <thread>  // NOLINT(build/c++11)

#if defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <sys/types.h>

#include <sys/sysctl.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace zkbench {

std::string GetOsName() {
#if defined(__linux__)
  return "linux";
#elif defined(__APPLE__)
  return "macos";
#elif defined(_WIN32)
  return "windows";
#else
  return "unknown";
#endif
}

std::string GetArchName() {
#if defined(__x86_64__) || defined(_M_X64)
  return "x86_64";
#elif defined(__aarch64__) || defined(_M_ARM64)
  return "aarch64";
#elif defined(__i386__) || defined(_M_IX86)
  return "x86";
#elif defined(__arm__) || defined(_M_ARM)
  return "arm";
#else
  return "unknown";
#endif
}

int GetCpuCount() {
  unsigned int count = std::thread::hardware_concurrency();
  return count > 0 ? static_cast<int>(count) : 1;
}

#if defined(__linux__)
std::optional<std::string> GetCpuVendorLinux() {
  std::ifstream cpuinfo("/proc/cpuinfo");
  if (!cpuinfo.is_open()) {
    return std::nullopt;
  }

  std::string line;
  std::regex model_name_regex(R"(model name\s*:\s*(.+))");
  std::smatch match;

  while (std::getline(cpuinfo, line)) {
    if (std::regex_search(line, match, model_name_regex)) {
      return match[1].str();
    }
  }
  return std::nullopt;
}
#endif

#if defined(__APPLE__)
std::optional<std::string> GetCpuVendorMacOS() {
  char buffer[256];
  size_t size = sizeof(buffer);
  if (sysctlbyname("machdep.cpu.brand_string", buffer, &size, nullptr, 0) ==
      0) {
    return std::string(buffer);
  }
  return std::nullopt;
}
#endif

#if defined(_WIN32)
std::optional<std::string> GetCpuVendorWindows() {
  const char* processor_id = std::getenv("PROCESSOR_IDENTIFIER");
  if (processor_id != nullptr) {
    return std::string(processor_id);
  }
  return std::nullopt;
}
#endif

std::optional<std::string> GetCpuVendor() {
#if defined(__linux__)
  return GetCpuVendorLinux();
#elif defined(__APPLE__)
  return GetCpuVendorMacOS();
#elif defined(_WIN32)
  return GetCpuVendorWindows();
#else
  return std::nullopt;
#endif
}

}  // namespace zkbench
