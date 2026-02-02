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

#include <map>
#include <mutex>  // NOLINT(build/c++11)
#include <string>

namespace zkbench {
namespace {

std::mutex& GetMutex() {
  static std::mutex mutex;
  return mutex;
}

std::map<std::string, TestVectors>& GetTestVectorsMap() {
  static std::map<std::string, TestVectors> test_vectors_map;
  return test_vectors_map;
}

std::map<std::string, nlohmann::json>& GetMetadataMap() {
  static std::map<std::string, nlohmann::json> metadata_map;
  return metadata_map;
}

}  // namespace

void BenchmarkContext::SetTestVectors(std::string_view benchmark_name,
                                      std::string_view input_hash,
                                      std::string_view output_hash,
                                      bool verified) {
  std::lock_guard<std::mutex> lock(GetMutex());
  GetTestVectorsMap()[std::string(benchmark_name)] = TestVectors{
      std::string(input_hash),
      std::string(output_hash),
      verified,
  };
}

void BenchmarkContext::SetMetadata(std::string_view benchmark_name,
                                   const nlohmann::json& metadata) {
  std::lock_guard<std::mutex> lock(GetMutex());
  GetMetadataMap()[std::string(benchmark_name)] = metadata;
}

std::optional<TestVectors> BenchmarkContext::GetTestVectors(
    std::string_view benchmark_name) {
  std::lock_guard<std::mutex> lock(GetMutex());
  auto& map = GetTestVectorsMap();
  auto it = map.find(std::string(benchmark_name));
  if (it != map.end()) {
    return it->second;
  }
  return std::nullopt;
}

nlohmann::json BenchmarkContext::GetMetadata(std::string_view benchmark_name) {
  std::lock_guard<std::mutex> lock(GetMutex());
  auto& map = GetMetadataMap();
  auto it = map.find(std::string(benchmark_name));
  if (it != map.end()) {
    return it->second;
  }
  return nlohmann::json::object();
}

void BenchmarkContext::Clear() {
  std::lock_guard<std::mutex> lock(GetMutex());
  GetTestVectorsMap().clear();
  GetMetadataMap().clear();
}

}  // namespace zkbench
