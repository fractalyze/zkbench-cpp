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

#include "zkbench/hash.h"

#include "openssl/sha.h"

namespace zkbench {

constexpr char kHexChars[] = "0123456789abcdef";

std::string ComputeHash(const uint8_t* data, size_t size) {
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256(data, size, digest);

  std::string hex(SHA256_DIGEST_LENGTH * 2, '\0');
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    hex[i * 2] = kHexChars[digest[i] >> 4];
    hex[i * 2 + 1] = kHexChars[digest[i] & 0x0f];
  }
  return hex;
}

}  // namespace zkbench
