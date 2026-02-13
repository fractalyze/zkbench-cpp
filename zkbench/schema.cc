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

#include "zkbench/schema.h"

#include "zkbench/platform.h"
#include "zkbench/utils.h"

namespace zkbench {

MetricValue MetricValue::Create(double value, std::string_view unit) {
  return MetricValue{value, std::string(unit), std::nullopt, std::nullopt};
}

MetricValue MetricValue::WithBounds(double value, std::string_view unit,
                                    double lower, double upper) {
  return MetricValue{value, std::string(unit), lower, upper};
}

Platform Platform::Current() {
  return Platform{
      GetOsName(), GetArchName(), GetCpuCount(), GetCpuVendor(), GetGpuVendor(),
  };
}

Metadata Metadata::Create(std::string_view implementation,
                          std::string_view version) {
  return Metadata{
      std::string(implementation), std::string(version),
      GetGitCommitSha(),           GetTimestamp(),
      Platform::Current(),
  };
}

std::string BenchmarkReport::ToJson(int indent) const {
  // Use ordered_json to preserve insertion order
  nlohmann::ordered_json j;
  j["metadata"] = nlohmann::ordered_json{
      {"implementation", metadata.implementation},
      {"version", metadata.version},
      {"commit_sha", metadata.commit_sha},
      {"timestamp", metadata.timestamp},
      {"platform",
       nlohmann::ordered_json{{"os", metadata.platform.os},
                              {"arch", metadata.platform.arch},
                              {"cpu_count", metadata.platform.cpu_count}}}};
  if (metadata.platform.cpu_vendor.has_value()) {
    j["metadata"]["platform"]["cpu_vendor"] =
        metadata.platform.cpu_vendor.value();
  }
  if (metadata.platform.gpu_vendor.has_value()) {
    j["metadata"]["platform"]["gpu_vendor"] =
        metadata.platform.gpu_vendor.value();
  }

  j["benchmarks"] = nlohmann::ordered_json::object();
  for (const auto& [name, result] : benchmarks) {
    nlohmann::ordered_json br;
    if (result.latency.has_value()) {
      nlohmann::ordered_json lat{{"value", result.latency->value},
                                 {"unit", result.latency->unit}};
      if (result.latency->lower_value.has_value()) {
        lat["lower_value"] = result.latency->lower_value.value();
      }
      if (result.latency->upper_value.has_value()) {
        lat["upper_value"] = result.latency->upper_value.value();
      }
      br["latency"] = lat;
    }
    if (result.memory.has_value()) {
      nlohmann::ordered_json mem{{"value", result.memory->value},
                                 {"unit", result.memory->unit}};
      br["memory"] = mem;
    }
    if (result.throughput.has_value()) {
      nlohmann::ordered_json thr{{"value", result.throughput->value},
                                 {"unit", result.throughput->unit}};
      br["throughput"] = thr;
    }
    if (result.iterations > 0) {
      br["iterations"] = result.iterations;
    }
    if (result.test_vectors.has_value()) {
      br["test_vectors"] = nlohmann::ordered_json{
          {"input_hash", result.test_vectors->input_hash},
          {"output_hash", result.test_vectors->output_hash},
          {"verified", result.test_vectors->verified}};
    }
    if (!result.metadata.empty()) {
      br["metadata"] = result.metadata;
    }
    j["benchmarks"][name] = br;
  }

  return j.dump(indent);
}

BenchmarkReport BenchmarkReport::FromJson(std::string_view json) {
  nlohmann::json j = nlohmann::json::parse(json);
  return j.get<BenchmarkReport>();
}

void to_json(nlohmann::json& j, const MetricValue& m) {
  j = nlohmann::json{{"value", m.value}, {"unit", m.unit}};
  if (m.lower_value.has_value()) {
    j["lower_value"] = m.lower_value.value();
  }
  if (m.upper_value.has_value()) {
    j["upper_value"] = m.upper_value.value();
  }
}

void to_json(nlohmann::json& j, const TestVectors& tv) {
  j = nlohmann::json{{"input_hash", tv.input_hash},
                     {"output_hash", tv.output_hash},
                     {"verified", tv.verified}};
}

void to_json(nlohmann::json& j, const BenchmarkResult& br) {
  j = nlohmann::json::object();
  if (br.latency.has_value()) {
    j["latency"] = br.latency.value();
  }
  if (br.memory.has_value()) {
    j["memory"] = br.memory.value();
  }
  if (br.throughput.has_value()) {
    j["throughput"] = br.throughput.value();
  }
  if (br.iterations > 0) {
    j["iterations"] = br.iterations;
  }
  if (br.test_vectors.has_value()) {
    j["test_vectors"] = br.test_vectors.value();
  }
  if (!br.metadata.empty()) {
    j["metadata"] = br.metadata;
  }
}

void to_json(nlohmann::json& j, const Platform& p) {
  j = nlohmann::json{
      {"os", p.os}, {"arch", p.arch}, {"cpu_count", p.cpu_count}};
  if (p.cpu_vendor.has_value()) {
    j["cpu_vendor"] = p.cpu_vendor.value();
  }
  if (p.gpu_vendor.has_value()) {
    j["gpu_vendor"] = p.gpu_vendor.value();
  }
}

void to_json(nlohmann::json& j, const Metadata& m) {
  j = nlohmann::json{{"implementation", m.implementation},
                     {"version", m.version},
                     {"commit_sha", m.commit_sha},
                     {"timestamp", m.timestamp},
                     {"platform", m.platform}};
}

void to_json(nlohmann::json& j, const BenchmarkReport& r) {
  j = nlohmann::json{{"metadata", r.metadata}, {"benchmarks", r.benchmarks}};
}

void from_json(const nlohmann::json& j, MetricValue& m) {
  j.at("value").get_to(m.value);
  j.at("unit").get_to(m.unit);
  if (j.contains("lower_value")) {
    m.lower_value = j.at("lower_value").get<double>();
  }
  if (j.contains("upper_value")) {
    m.upper_value = j.at("upper_value").get<double>();
  }
}

void from_json(const nlohmann::json& j, TestVectors& tv) {
  j.at("input_hash").get_to(tv.input_hash);
  j.at("output_hash").get_to(tv.output_hash);
  j.at("verified").get_to(tv.verified);
}

void from_json(const nlohmann::json& j, BenchmarkResult& br) {
  if (j.contains("latency")) {
    br.latency = j.at("latency").get<MetricValue>();
  }
  if (j.contains("memory")) {
    br.memory = j.at("memory").get<MetricValue>();
  }
  if (j.contains("throughput")) {
    br.throughput = j.at("throughput").get<MetricValue>();
  }
  if (j.contains("iterations")) {
    j.at("iterations").get_to(br.iterations);
  }
  if (j.contains("test_vectors")) {
    br.test_vectors = j.at("test_vectors").get<TestVectors>();
  }
  if (j.contains("metadata")) {
    br.metadata = j.at("metadata");
  }
}

void from_json(const nlohmann::json& j, Platform& p) {
  j.at("os").get_to(p.os);
  j.at("arch").get_to(p.arch);
  j.at("cpu_count").get_to(p.cpu_count);
  if (j.contains("cpu_vendor")) {
    p.cpu_vendor = j.at("cpu_vendor").get<std::string>();
  }
  if (j.contains("gpu_vendor")) {
    p.gpu_vendor = j.at("gpu_vendor").get<std::string>();
  }
}

void from_json(const nlohmann::json& j, Metadata& m) {
  j.at("implementation").get_to(m.implementation);
  j.at("version").get_to(m.version);
  j.at("commit_sha").get_to(m.commit_sha);
  j.at("timestamp").get_to(m.timestamp);
  j.at("platform").get_to(m.platform);
}

void from_json(const nlohmann::json& j, BenchmarkReport& r) {
  j.at("metadata").get_to(r.metadata);
  j.at("benchmarks").get_to(r.benchmarks);
}

}  // namespace zkbench
