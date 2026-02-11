# Copyright 2026 The zkbench-cpp Authors.
# SPDX-License-Identifier: Apache-2.0

"""BoringSSL external dependency for WORKSPACE-based builds."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

VERSION = "0.20260204.0"

def repo():
    http_archive(
        name = "boringssl",
        url = "https://github.com/google/boringssl/releases/download/{version}/boringssl-{version}.tar.gz".format(version = VERSION),
        strip_prefix = "boringssl-{version}".format(version = VERSION),
        integrity = "sha256-b2S/Hkr40UFEypMfiIH/iQ0BwkCt1Rc1foxU9LjXm2U=",
    )
