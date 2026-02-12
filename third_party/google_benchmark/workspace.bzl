# Copyright 2026 The zkbench-cpp Authors.
# SPDX-License-Identifier: Apache-2.0

"""Google Benchmark external dependency for WORKSPACE-based builds."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

VERSION = "1.9.4"

def repo():
    http_archive(
        name = "com_google_benchmark",
        url = "https://github.com/google/benchmark/archive/refs/tags/v{version}.tar.gz".format(version = VERSION),
        strip_prefix = "benchmark-{version}".format(version = VERSION),
        integrity = "sha256-szRljt01788GqZ2b4h5Ok+CSvV+VB0wWc9XIcF2VwQQ=",
    )
