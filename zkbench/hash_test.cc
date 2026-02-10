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

#include <cstdint>

#include "gtest/gtest.h"

namespace zkbench {
namespace {

TEST(ComputeHashTest, EmptyInput) {
  EXPECT_EQ(ComputeHash(nullptr, 0),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(ComputeHashTest, Abc) {
  const uint8_t data[] = {'a', 'b', 'c'};
  EXPECT_EQ(ComputeHash(data, sizeof(data)),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST(ComputeArrayHashTest, Uint32Array) {
  const uint32_t data[] = {1, 2, 3};
  EXPECT_EQ(ComputeArrayHash(data, 3),
            "4636993d3e1da4e9d6b8f87b79e8f7c6d018580d52661950eabc3845c5897a4d");
}

TEST(ComputeArrayHashTest, SameDataSameHash) {
  const uint32_t a[] = {10, 20, 30};
  const uint32_t b[] = {10, 20, 30};
  EXPECT_EQ(ComputeArrayHash(a, 3), ComputeArrayHash(b, 3));
}

TEST(ComputeArrayHashTest, DifferentDataDifferentHash) {
  const uint32_t a[] = {1, 2, 3};
  const uint32_t b[] = {1, 2, 4};
  EXPECT_NE(ComputeArrayHash(a, 3), ComputeArrayHash(b, 3));
}

}  // namespace
}  // namespace zkbench
