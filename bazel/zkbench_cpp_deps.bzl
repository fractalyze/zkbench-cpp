# Copyright 2026 The zkbench-cpp Authors.
# SPDX-License-Identifier: Apache-2.0

"""External dependencies for WORKSPACE-based consumers of zkbench-cpp."""

load("//third_party/boringssl:workspace.bzl", boringssl = "repo")
load("//third_party/google_benchmark:workspace.bzl", google_benchmark = "repo")

def zkbench_cpp_deps():
    """Sets up all external dependencies required by zkbench-cpp."""
    boringssl()
    google_benchmark()
