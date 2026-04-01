# zkbench-cpp

C++ benchmarking library with JSON schema compatible with
[zkbench-py](https://github.com/fractalyze/zkbench-py) and
[zkbench-rust](https://github.com/fractalyze/zkbench-rust).

## Features

- Common JSON schema for cross-implementation benchmark results
- Platform detection (OS, architecture, CPU info)
- Statistical calculations (mean, stdev, confidence intervals)
- Stopwatch utility for timing
- Integration with Google Benchmark via custom reporter

## Usage

### Bazel

Add to your `MODULE.bazel`:

```starlark
bazel_dep(name = "zkbench_cpp")
git_override(
    module_name = "zkbench_cpp",
    remote = "https://github.com/fractalyze/zkbench-cpp",
    commit = "<commit-sha>",
)
```

Then depend on `@zkbench_cpp//zkbench`:

```starlark
cc_library(
    name = "my_benchmark",
    deps = ["@zkbench_cpp//zkbench"],
)
```

### Code Example

```cpp
#include "zkbench/schema.h"
#include "zkbench/stopwatch.h"
#include "zkbench/statistics.h"

int main() {
  // Create a benchmark report
  zkbench::BenchmarkReport report;
  report.metadata = zkbench::Metadata::Create("my-implementation", "1.0.0");

  // Run benchmarks and collect timings
  std::vector<double> times;
  for (int i = 0; i < 10; ++i) {
    zkbench::Stopwatch sw;
    sw.Start();
    // ... your benchmark code ...
    sw.Stop();
    times.push_back(sw.ElapsedMillis());
  }

  // Calculate statistics
  auto [mean, stdev] = zkbench::CalculateStatistics(times);
  auto [lower, upper] = zkbench::CalculateConfidenceInterval(mean, stdev,
                                                              times.size());

  // Store results
  zkbench::BenchmarkResult result;
  result.latency = zkbench::MetricValue::WithBounds(mean, "ms", lower, upper);
  result.iterations = times.size();
  report.benchmarks["my_benchmark"] = result;

  // Output JSON
  std::cout << report.ToJson() << std::endl;
  return 0;
}
```

## JSON Schema

Output is compatible with zkbench-py and zkbench-rust:

```json
{
  "metadata": {
    "implementation": "my-implementation",
    "version": "1.0.0",
    "commit_sha": "abc123def456",
    "timestamp": "2026-02-03T12:00:00Z",
    "platform": {
      "os": "linux",
      "arch": "x86_64",
      "cpu_count": 32,
      "cpu_vendor": "AMD Ryzen 9 9950X3D"
    }
  },
  "benchmarks": {
    "my_benchmark": {
      "latency": {
        "value": 12.5,
        "unit": "ms",
        "lower_value": 10.0,
        "upper_value": 15.0
      },
      "iterations": 10
    }
  }
}
```

## Building

```bash
bazel build //zkbench/...
```

## Testing

```bash
bazel test //zkbench/...
```

## License

Apache-2.0
