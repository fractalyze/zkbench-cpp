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

#ifndef ZKBENCH_HASH_H_
#define ZKBENCH_HASH_H_

#include <cstddef>
#include <cstdint>
#include <string>

namespace zkbench {

/// Computes the SHA-256 hash of raw bytes.
///
/// @param data Pointer to the input data.
/// @param size Size of the input data in bytes.
/// @return 64-character lowercase hex string of the SHA-256 digest.
std::string ComputeHash(const uint8_t* data, size_t size);

/// Computes the SHA-256 hash of a typed array.
///
/// Reinterprets the array as raw bytes (little-endian) and computes SHA-256.
///
/// @tparam T Element type of the array.
/// @param data Pointer to the array.
/// @param count Number of elements in the array.
/// @return 64-character lowercase hex string of the SHA-256 digest.
template <typename T>
std::string ComputeArrayHash(const T* data, size_t count) {
  return ComputeHash(reinterpret_cast<const uint8_t*>(data), count * sizeof(T));
}

}  // namespace zkbench

#endif  // ZKBENCH_HASH_H_
